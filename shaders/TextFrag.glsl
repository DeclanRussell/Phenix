#version 400
uniform sampler2D tex;
in vec2 vertUV;
out vec4 fragColour;
uniform vec3 textColour;

void main()
{
    vec4 text=texture(tex,vertUV.st-vec2(0,0.1)); // -0.1 in y is a quick hack to get rid of lines that appear above letters on windows
    fragColour.rgb=textColour.rgb;
    fragColour.a=text.a;
}
