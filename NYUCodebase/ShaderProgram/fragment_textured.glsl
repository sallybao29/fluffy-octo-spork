
uniform sampler2D diffuse;
uniform float alpha;
varying vec2 texCoordVar;

void main() {
    vec4 finalColor = texture2D(diffuse, texCoordVar);
    if (finalColor.a == 0.0) {
        discard;
    }
    finalColor.a = alpha;
    gl_FragColor = finalColor;
}
