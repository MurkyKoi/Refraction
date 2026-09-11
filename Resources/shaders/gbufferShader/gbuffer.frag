#version 450

layout (location = 0) out vec3 gDiffuse;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gPosition;
layout (location = 3) out vec3 gSMR;
layout (location = 4) out float gDepth;
layout (location = 5) out vec3 gCFAAData;

in VERT_OUT {
	vec4 FragColor;
	vec3 FragPos;
	vec2 TexCoords;
	vec2 ScreenTexCoords;
	vec3 Normal;
} VertOut;

uniform bool usingCFAA;
uniform int CFAAScale;
uniform float viewNear = 0.01;
uniform float viewFar = 1000.0;

// Model textures
uniform sampler2D tDiffuse;
uniform sampler2D tSpecular;

// CFAA prepass data
uniform sampler2D tCFAADiffuse;
uniform sampler2D tCFAANormal;
uniform sampler2D tCFAAPosition;
uniform sampler2D tCFAASMR;
uniform sampler2D tCFAADepth;

float LineariseDepth(float rawDepth) {
	float z = rawDepth * 2.0 - 1.0;
	return (2.0 * viewNear * viewFar) / (viewFar + viewNear - z * (viewFar - viewNear));
}

void main() {
	if(usingCFAA) { // Use CFAA prepass data, expect to sample to upscaled image
		vec2 nativeSize = vec2(textureSize(tCFAADepth, 0));
		vec2 upscaledSize = nativeSize * float(CFAAScale);
		vec2 correctFragCoord = vec2(gl_FragCoord.x, upscaledSize.y - gl_FragCoord.y);

		vec2 sampleCoords = correctFragCoord / upscaledSize;
		vec2 nativeTexelSize = 1.0 / nativeSize;

		// Get depth contrast
		float maxDepthDelta = 0.0;
		float centerRawDepth = texture(tCFAADepth, sampleCoords).r;
		float centerLinearDepth = LineariseDepth(centerRawDepth);

		int kernelSize = 1; // Evaluates immediate neighbor cells
		for(int x = -kernelSize; x <= kernelSize; x++) {
			for(int y = -kernelSize; y <= kernelSize; y++) {
				if(x == 0 && y == 0) continue;

				float neighborRawDepth = texture(tCFAADepth, sampleCoords + vec2(nativeTexelSize.x * x, nativeTexelSize.y * y)).r;
				float neighborLinearDepth = LineariseDepth(neighborRawDepth);

				float delta = abs(neighborLinearDepth - centerLinearDepth);
				if (delta > maxDepthDelta) {
					maxDepthDelta = delta;
				}
			}
		}

		bool isHighContrast = (maxDepthDelta > 0.05);

		if (isHighContrast) {
			float visibleContrast = clamp(maxDepthDelta * 5.0, 0.0, 1.0);
			gCFAAData = vec3(visibleContrast, 0.0, 0.0);

			gDiffuse = texture(tDiffuse, VertOut.TexCoords).rgb;
			gNormal = normalize(VertOut.Normal);
			gPosition = VertOut.FragPos;
			gSMR = vec3(texture(tSpecular, VertOut.TexCoords).r, 0.0, 0.0);
			gDepth = gl_FragCoord.z;
		} else {
			gCFAAData = vec3(0.0, 0.0, 0.0);

			gDiffuse = texture(tCFAADiffuse, sampleCoords).rgb;
			gNormal = texture(tCFAANormal, sampleCoords).rgb;
			gPosition = texture(tCFAAPosition, sampleCoords).rgb;
			gSMR = texture(tCFAASMR, sampleCoords).rgb;
			gDepth = texture(tCFAADepth, sampleCoords).r;
		}
	} else {
		gDiffuse = texture(tDiffuse, VertOut.TexCoords).rgb;
		gNormal = normalize(VertOut.Normal);
		gPosition = VertOut.FragPos;
		gSMR = vec3(texture(tSpecular, VertOut.TexCoords).r, 0.0, 0.0);
		gDepth = gl_FragCoord.z;
		gCFAAData = vec3(0.0);
	}
}