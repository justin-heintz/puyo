	#version 330 core
	in vec2 TexCoords;

	uniform sampler2D textureIn;
	uniform vec4 colorIn;
	out vec4 color;

	float blendChannel(float base, float blend){
 		if (blend < 0.5) {
			return 2.0 * base * blend + base*base - 2.0 * base*base*blend;
		} else {
			return 2.0 * sqrt(base) * blend - sqrt(base) + 2.0 * base - 2.0 * base*blend;
		}
	}
	
	void main(){
		vec4 v4_color = texture2D(textureIn, TexCoords);
		color = vec4( blendChannel(v4_color.r,colorIn.r), blendChannel(v4_color.b,colorIn.b), blendChannel(v4_color.g,colorIn.g), blendChannel(v4_color.a,colorIn.a) );
	//color = v4_color;
	} 