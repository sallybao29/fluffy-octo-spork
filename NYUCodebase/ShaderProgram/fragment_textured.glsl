
uniform sampler2D diffuse;
uniform float alpha;
varying vec2 texCoordVar;

float mapValue(float value, float srcMin, float srcMax, float dstMin, float dstMax) {
    float retVal = dstMin + ((value - srcMin)/(srcMax-srcMin) * (dstMax-dstMin));
    if(retVal < dstMin) {
        retVal = dstMin;
    }
    if(retVal > dstMax) {
        retVal = dstMax;
    }
    return retVal;
}

void main() {
    vec4 finalColor = texture2D(diffuse, texCoordVar);
    finalColor.a = mapValue(alpha, 0.0, 1.0, finalColor.a, 1.0);
    gl_FragColor = finalColor;
}
