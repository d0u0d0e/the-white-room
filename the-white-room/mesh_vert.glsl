attribute vec3 aPosition;
attribute vec3 aNormal;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;

uniform vec3 uLightPos;
uniform vec3 uLightColor;

varying vec3 vNormal;
varying vec3 vLightDir;
varying vec3 vThePosition;

void main() {
  vec4 vPosition;

  /* First model transforms */
  vPosition = uModelMatrix * vec4(aPosition, 1);
  vThePosition = vPosition.xyz;

  vLightDir = normalize(uLightPos - vec3(vPosition));
  vNormal = normalize(uNormalMatrix * vec4(aNormal, 0)).xyz;

  vPosition = uViewMatrix * vPosition;
  gl_Position = uProjMatrix * vPosition;
}
