
attribute vec3 aVertexPosition;
attribute vec3 aVertexNormal;
attribute vec2 aTextureCoord;
attribute vec4 aVertexColor;
attribute vec2 aHeightTexCoord;

uniform sampler2D uSampler;
uniform sampler2D uHeightMap;
uniform vec2 uShipCoord;
uniform float uAntennaHeight;
uniform float uBeamHeight2;
uniform float uRange;
uniform float uWaveHeight;
uniform float uWindDirection;
uniform float uRotAngle;
uniform float uSea;
uniform float uAngle0;

uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat3 uNMatrix;
uniform vec3 uLightPosition;
uniform int uFlag1;

varying vec2 vTextureCoord;
varying vec2 vHeightTexCoord;
varying vec4 vColor;
varying float vLightWeighting;
varying float dx;
varying float dy;
varying float vRange;
varying float vAngleCoef;
varying float vHeight;
varying vec2 vFragPos;
varying vec4 vFragPos1;

const float PI = 3.14159265358979;

float rseed = 1558911735.0;

/*
float rand(float x, float y){
    return fract(sin(x * 12.9898 + y * 78.233) * 43758.5453);
}
                                // from -1.0 to +1.0
float random()
{
  rseed = rand(rseed,rseed / 13.0);
  return (1.0 + rseed) * 0.5;
}
*/

float GetAngle(float dx, float dy)
{
	float a = 0.0;
	if (abs(dy) < 0.000000001)
	{
		if (dx < 0.0) a = 180.0;
			else a = 0.0;
	} else
	{
		if (dy >= 0.0)
		{
			if (dx >= 0.0)
			{
				a = (atan(dx / dy) * 180.0 / PI);
			} else
			{
				a = (atan(dx / dy) * 180.0 / PI) + 360.0;
			}
		} else
		{
			a = (atan(dx / dy) * 180.0 / PI) + 180.0;
		}
	}
	
	return a;
}



void main(void) {

		// gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition,1.0);
  	dx = aVertexPosition[0] - uLightPosition[0];
	dy = aVertexPosition[1] - uLightPosition[1];	
	vRange = uRange;
		                            // normalise angle and lighting directions
	float an = uAngle0 * PI / 180.0;
    vec3 angleDirection = vec3(sin(an),cos(an),0.0);
                              // cross-product
	vec3 lightDirection = normalize(vec3(dx,dy,0));						  
    vec3 p = cross(angleDirection,lightDirection);
                              // angle b/w vectors
    float s = dot(angleDirection,lightDirection);
    float ang = acos(s) * 180.0 / PI;
                              // bring to 0..360
    if (p[2] < 0.0) ang = 360.0 - ang;
                              // bring to 0..1
    vAngleCoef = 0.5 + (1.0 - ang / 360.0) * 0.5;

    vFragPos = aVertexPosition.xy;

	if (uFlag1 == 0)
	{
		gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition,1.0);
        vFragPos1 = gl_Position;
        vec3 xyz = vec3(0.0-gl_Position.x, 0.0-gl_Position.y, 30.0 - gl_Position.z);
		// vec3 normal = normalize(aVertexNormal);
		vec3 normal = normalize(xyz);
		vLightWeighting = -dot(normal,lightDirection);
        vHeight = aVertexPosition.z;
		
	} else if (uFlag1 == 1)
	{
		gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition,1.0);
		vTextureCoord = aTextureCoord;
	} else if (uFlag1 == 2)
	{
		gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition,1.0);
		
		vec3 normal = normalize(vec3(0.0, 0.0, 1.0));
								// wave addition to vLightWeighting
		float waveweighting = 0.0;
		
		if (uWaveHeight > 0.0)
		{
								// approximate
			float wavelen = uWaveHeight * 10.0;
								// distance to centre
			float rr = sqrt(dx * dx + dy * dy);
								// random phase
			//float r0 = wavelen * random();
								// sine
			//float s = sin((r0 + rr) * 2.0 * PI / wavelen);
								// get beam angle with heading angle
			float a = GetAngle(dy,dx) + uWindDirection;
								// distance coef (fading from centre)
								// r1 must be 4000.0 as well
			float rcoef = 1.0 - rr / 4000.0;
			if (rcoef < 0.0) rcoef = 0.0;
								// min/max addition to dot product
			float min = 0.01;
			float max = 0.5 * uWaveHeight / 10.0;
			
			waveweighting = (min + (1.0 + sin(a * PI / 180.0)) * 0.5 * max) * pow(rcoef,1.0 + uSea * 4.0);
		}
		
		vLightWeighting = -dot(normal,lightDirection) + waveweighting;
	} else if (uFlag1 == 3)
	{
		gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition,1.0);
		
		float height = (uLightPosition[2] > 0.0) ? uLightPosition[2] : 10.0;
		float coef = aVertexPosition[2] / height;
		if (coef > 1.0) coef = 1.0;
		
	//	vec3 normal = normalize(aVertexNormal);
	//	vec3 lightingDirection = normalize(aVertexPosition - uLightPosition);
		vLightWeighting = coef;

		//gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition,1.0);
		//vec3 normal = normalize(aVertexNormal);
		//vec3 lightingDirection = normalize(aVertexPosition - uLightPosition);
		//vLightWeighting = 1.0;
	} else if (uFlag1 == 4)
    {
		gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition,1.0);
		vHeightTexCoord = aHeightTexCoord;
    }
}
