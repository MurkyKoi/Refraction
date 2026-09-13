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

// Model textures
uniform sampler2D tDiffuse;
uniform sampler2D tSpecular;

// CFAA prepass data
uniform sampler2D tCFAADiffuse;
uniform sampler2D tCFAANormal;
uniform sampler2D tCFAAPosition;
uniform sampler2D tCFAASMR;
uniform sampler2D tCFAADepth;

void main() {
	if(usingCFAA) { // Use CFAA prepass data, expect to sample to upscaled image
		vec2 nativeSize = vec2(textureSize(tCFAADiffuse, 0));
		vec2 prepassFragCoord = vec2(gl_FragCoord.x / float(CFAAScale), gl_FragCoord.y / float(CFAAScale));

		vec2 sampleCoords = prepassFragCoord / nativeSize;
		vec2 nativeTexelSize = 1.0 / nativeSize;

		// Calculate contrast here because we wouldn't have the full image in the prepass shader

		// Get normal contrast
		float maxNormalDiff = 0.0;
		vec3 centerNormal = texture(tCFAANormal, sampleCoords).xyz;

		int kernelSize = CFAAScale;
		for(int x = -kernelSize; x <= kernelSize; x++) {
			for(int y = -kernelSize; y <= kernelSize; y++) {
				if(x == 0 && y == 0) continue;
				vec3 neighborNormal = texture(tCFAANormal, sampleCoords + vec2(nativeTexelSize.x * x, nativeTexelSize.y * y)).xyz;
				float variance = 1.0 - max(dot(centerNormal, neighborNormal), 0.0);
				if (variance > maxNormalDiff) {
					maxNormalDiff = variance;
				}
			}
		}

		bool isHighContrast = (maxNormalDiff > 0.01);

		if (isHighContrast) { // Supersample
			float visibleContrast = clamp(maxNormalDiff * 10.0, 0.0, 1.0);
			gCFAAData = vec3(visibleContrast, 0.0, 0.0);

			gDiffuse = texture(tDiffuse, VertOut.TexCoords).rgb;
			gNormal = normalize(VertOut.Normal);
			gPosition = VertOut.FragPos;
			gSMR = vec3(texture(tSpecular, VertOut.TexCoords).r, 0.0, 0.0);
			gDepth = gl_FragCoord.z;
		} else { // Continue with prepass data
			gCFAAData = vec3(0.0, 1.0, 0.0);

			gDiffuse = texture(tCFAADiffuse, sampleCoords).rgb;
			gNormal = texture(tCFAANormal, sampleCoords).rgb;
			gPosition = texture(tCFAAPosition, sampleCoords).rgb;
			gSMR = texture(tCFAASMR, sampleCoords).rgb;
			gDepth = texture(tCFAADepth, sampleCoords).r;
		}
	} else { // Sample normally
		gDiffuse = texture(tDiffuse, VertOut.TexCoords).rgb;
		gNormal = normalize(VertOut.Normal);
		gPosition = VertOut.FragPos;
		gSMR = vec3(texture(tSpecular, VertOut.TexCoords).r, 0.0, 0.0);
		gDepth = gl_FragCoord.z;
		gCFAAData = vec3(0.0);
	}
}