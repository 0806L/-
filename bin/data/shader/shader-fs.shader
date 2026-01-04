#version 330
// precision highp float;
varying vec2 vTextureCoord;
varying vec4 vColor;
varying float vLightWeighting;
varying float dx;
varying float dy;
varying float vRange;
varying float vAngleCoef;
varying float vHeight;
varying vec2 vFragPos;
varying vec4 vFragPos1;
varying vec2 vHeightTexCoord;

uniform int uFlag;
uniform sampler2D uSampler;
uniform sampler2D uHeightMap;
uniform vec3 uDirectionalColor;
uniform vec2 uShipCoord;
uniform float uAntennaHeight;
uniform float uAngle0;
uniform float uAngle1;
uniform float uBrilliance;
uniform float uGain;
uniform float uTune;
uniform float uSea;
uniform float uRain;
uniform float uFTC;

const float PI = 3.1415926;

float GetAngle(float dx, float dy)
{
	float a = 0.0;
	if (abs(dy) < 0.000001)
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


// 噪声函数（简化版）
float hash(float n) { 
    return fract(sin(n)*43758.5453); 
}

float noise(vec2 x) {
    vec2 p = floor(x);
    vec2 f = smoothstep(0.0,1.0,fract(x));
    float n = p.x + p.y*57.0;
    return mix(mix(hash(n), hash(n+1.0),f.x),
               mix(hash(n+57.0), hash(n+58.0),f.x),f.y);
}


// 极坐标转换
vec2 polarCoord(vec2 pos) {
    return vec2(atan(pos.y, pos.x), length(pos));
}


// 片段着色器中的增益控制模块
vec3 advancedGainControl(vec3 rawSignal, float distance) {
    // ========== 参数配置 ==========
    const float MAX_GAIN = 60.0;      // 最大增益 (dB)
    const float STC_SLOPE = 0.2;      // STC衰减斜率 (dB/km)
    const float LOG_OFFSET = 1.0;     // 对数压缩偏移量
    const float AGC_TIME_CONST = 0.3; // AGC时间常数
    
    // ========== 信号预处理 ==========
    // 1. 基础线性增益
    vec3 amplified = rawSignal * pow(10.0, uGain * 0.05); // dB转线性
    
    // ========== STC距离补偿 ==========
    // 灵敏度时间控制（随距离增加衰减）
    float stcAttenuation = STC_SLOPE * distance * 0.001; // 距离转km
    float stcCompensation = pow(10.0, stcAttenuation * 0.05); 
    amplified *= stcCompensation;

    /*
    // ========== 自动增益控制 ==========
    // 估计当前信号水平（使用空间平均替代时间平均）
    vec3 avgIntensity = textureLod(uAvgTex, TexCoord, 3.0).rgb;
    vec3 agcFactor = 1.0 / (avgIntensity + 0.001);
    
    // AGC渐进调整
    agcFactor = mix(vec3(1.0), agcFactor, AGC_TIME_CONST);
    amplified *= clamp(agcFactor, 0.1, 10.0);
    */

    // ========== 动态范围压缩 ==========
    // 对数压缩（保留弱信号细节）
    vec3 logCompressed = log(amplified + LOG_OFFSET) / log(MAX_GAIN);
    
    // ========== 非线性增益曲线 ==========
    // 分段增益曲线：弱信号放大，强信号限幅
    vec3 processed = logCompressed * 1.5; // 低强度区域增益提升
    processed = clamp(processed, 0.0, 1.0);
    
    // 高频细节增强
    // vec3 hiFreq = processed - textureLod(uBlurTex, TexCoord, 2.0).rgb;
    // processed += hiFreq * 0.3;

    return processed;
}

vec3 gainfun1()
{

    vec2 st = polarCoord(vFragPos1.xy);
    float scanAngle = uAngle0*PI/180.0;
    float gain = uGain;

    float signal = 1.0 - exp(-4.0 * gain);
    signal = max(signal - 0.1 * (1.0 - gain), 0.0);

    // 黄色生成
    vec3 baseColor = vec3(1.0, 1.0, 0.0); // RGB黄色
    float brightness = smoothstep(0.2, 0.8, signal); 
    vec3 finalColor = baseColor * brightness;

    // 距离衰减
    float dist = st.y;
    float distFac = 1.0 - exp(-6.0*uGain);
    finalColor *= (1.0 - smoothstep(0.5*distFac, 1.0*distFac, dist));

    return finalColor;
}


// 随机噪声生成
float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898,78.233))) * 43758.5453);
}


// 分形噪声
float fractal_noise(vec2 uv) {
    float n = 0.0;
    float a = 0.5;
    for(int i=0; i<3; ++i) {
        n += a * rand(uv);
        uv *= 2.0;
        a *= 0.5;
    }
    return n;
}

vec3 gainfun2()
{
    // 基础信号采样
    float rawSignal = 1.0;
    float gain = uGain;
    float processedSignal = rawSignal * gain * 2.0; // 信号放大
    
    // 噪声生成
    float time = 0.1;
    vec2 noiseUV = vFragPos1.xy * vec2(1920.0/1080.0, 1.0) + time*0.1;
    float noise = fractal_noise(noiseUV * 50.0);
    
    // 信号处理管线
    if(gain < 0.3) {
        // 低增益模式
        float noiseThreshold = 0.3;
        processedSignal = max(processedSignal - noiseThreshold, 0.0);
        processedSignal *= 0.5; // 亮度抑制
        noise *= 0.1; // 抑制噪声
    } 
    else if(gain < 0.7) {
        // 中增益模式 - 非线性压缩
        processedSignal = 1.0 - exp(-processedSignal * 0.8);
        noise *= 0.15;
        
        // 距离相关衰减
        float dist = length(vFragPos1.xy - vec2(0.5));
        processedSignal *= 1.0 - smoothstep(0.4, 0.8, dist);
    }
    else {
        // 高增益模式
        noise *= 0.3 + gain*0.7; // 噪声放大
        processedSignal = min(processedSignal, 1.2); // 强制限幅
        
        // 距离衰减
        float dist = length(vFragPos1.xy - vec2(0.5));
        float attenuation = 1.0 / (1.0 + dist * 15.0);
        processedSignal *= attenuation;
    }

    // 信号合成
    float finalSignal = processedSignal + noise;
    finalSignal = clamp(finalSignal, 0.0, 1.0);

    // 颜色映射（示例：冷色到暖色渐变）
    vec3 color = mix(vec3(0.0, 0.2, 0.8), vec3(1.0, 0.9, 0.0), finalSignal);
    
    // 高增益过曝效果
    if(gain >= 0.7 && finalSignal > 0.9) {
        color = mix(color, vec3(1.0), smoothstep(0.9, 1.0, finalSignal));
    }

    return color;
}


// 替换原有的线性增益计算
// 原代码：float effectiveZ = z * gain;
// 新非线性增益算法
/*
float nonlinearGain(float z, float gain) {
    // 指数曲线参数（可调节）
    const float base = 2.718; // e 自然常数
    const float exponent = 2.0; 
    
    // 增益响应曲线：前30%增益线性增长，后70%指数增长
    if(gain < 0.3) {
        return z * (1.0 + gain * 2.0); // 初始线性区
    } else {
        float normalizedGain = (gain - 0.3) / 0.7; // 标准化到0-1
        return z * (pow(base, exponent * normalizedGain) + 0.6);
    }
}
*/


// 非线性增益函数（三阶段）
float nonlinearGain(float z, float gain) {
    float g = 1.0 - gain;
    if(g < 0.3) { // 第一阶段：缓动增长
        float t = g / 0.3;
        t = t * t * (3.0 - 2.0 * t);
        return z * mix(1.0, 1.5, t);
    } else if(g < 0.7) { // 第二阶段：二次加速
        float t = (g - 0.3) / 0.4;
        return z * (1.5 + 3.0 * t * t);
    } else { // 第三阶段：微调饱和
        float t = (g - 0.7) / 0.3;
        return z * (4.5 + 0.2 * t);
    }
}


float noiseProb(float g) {

    if(g > 0.7) {
        return 1.0 - exp(-15.0*(g-0.7)); 
    } else if(g > 0.3) { 
        return 1.0 - smoothstep(0.3,0.7,g);
    } else { 
        return 0.0; 
    }
}


void main(void) {
  
    // fragColor = vec4(1.0, 0.5, 0.2, 1.0);
    // gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
	float gain = uGain * 2.0 * (1.0 - abs(uTune - 0.67) * 0.3);
	float FTCcoef = 8.0 * (1.0 + uFTC * 4.0);
  
	if (uFlag == 0)
	{
		float r = sqrt(dx * dx + dy * dy);
		if (r > vRange)
		{
			discard;
		} else
		{
            float h = 30.0;
            float noiseThreshold = 0.7;

            // 应用非线性增益
            float effectiveZ = vHeight*smoothstep(0.0, 0.7, uGain);

            // 调谐清晰度控制
            float clarity = 1.0 - abs(dFdx(vHeight) + dFdy(vHeight)) * uTune * 10.0;

            if (effectiveZ <= h)
            {
                /*
                if (uGain < noiseThreshold)
                {
                    discard;
                }
                else
                */
                {
                    // 噪点概率与增益强度正相关
                    // float noiseProb = smoothstep(0.0, 0.3, uGain - noiseThreshold);
                    // float noiseProb = clamp((uGain - noiseThreshold) * 0.2, 0.0, 0.3);
                    float noiseProb = 1.0 - exp(-0.01*(uGain - 0.5));
                    float time = 1.0;
                    if (rand(vFragPos1.xy) < noiseProb) 
                    {
                        // 生成随机噪点（黄色调）
                        vec4 color = vec4(1.0, 1.0, 0.0, 1.0);
                        gl_FragColor = color * clarity;
                    }
                    else
                    {
                        discard;
                    }
                }
            }
            else
            {
                // vec3 color = gainfun1();
                gl_FragColor = vec4(uDirectionalColor*clarity, 1.0);
            }


            /*
			float coef = vAngleCoef;
			//if (a < uAngle0 || a > uAngle1)
			//	coef = 0.75;

            // gl_FragColor = vec4(uDirectionalColor * coef * uBrilliance * gain * FTCcoef,1.0);

			if (vLightWeighting <= (1.0 - gain) + 0.003) 
			{
				discard;
			} else
			{
				gl_FragColor = vec4(uDirectionalColor * vLightWeighting * coef * uBrilliance * gain * FTCcoef,1.0);
			}
            */
		}
	} else if (uFlag == 1)
	{
		float r = sqrt(dx * dx + dy * dy);
		if (r > vRange)
		{
			discard;
		} else
		{
			vec4 c = texture2D(uSampler,vTextureCoord);
			if (c[0] == 0.0 && c[1] == 0.0 && c[2] == 0.0)
			{
				discard;
			} else
			{
				gl_FragColor = c;
			}
		}
	} else if (uFlag == 2)
	{
		float r = sqrt(dx * dx + dy * dy);
		if (r > vRange)
		{
			discard;
		} else
		{
			float a = GetAngle(dx,dy);
			
			float coef = vAngleCoef;
			//if (a < uAngle0 || a > uAngle1)
			//	coef = 0.75;

			if (vLightWeighting <= 0.0) 
			{
				discard;
			} else
			{
				gl_FragColor = vec4(uDirectionalColor * vLightWeighting * gain * coef * uBrilliance * 16.0,1.0); 
			}
		}
	}
	else if (uFlag == 4)
    {
		float r = sqrt(dx * dx + dy * dy);
		if (r > vRange)
		{
			discard;
		} else
		{
            // vec4 basec = texture2D(uHeightMap, uShipCoord);
            // float base_height = (basec.g*255.0 + basec.r)*255.0;
            // float base_height = 12.0;

            const int MAX_STEPS = 50;     // 最大步进次数 (平衡性能与精度)
            const float EPSILON = 0.001;  // 浮点误差阈值

            // 计算射线方向
            vec2 dir = vHeightTexCoord - uShipCoord;
            float rayLength = length(dir);
            if (rayLength < EPSILON) {
                gl_FragColor = vec4(1.0); // 中心点直接显示
                return;
            }
            vec2 rayDir = dir / rayLength; // 单位化方向

            // vec2 texsize = textureSize(uHeightMap);
            vec2 texsize = vec2(1.0/50000.0, 1.0/45000.0);

            // 射线步进参数
            // float stepSize = max(texsize.x, texsize.y) * 2.0; // 自适应步长
            float stepSize = 0.001;
            float travelDistance = stepSize;
            bool isOccluded = false;

			vec4 c = texture2D(uHeightMap,vHeightTexCoord);
            float height = (c.g*255.0 + c.r)*255.0;

            // 沿射线向雷达中心步进检测
            for(int i = 0; i < MAX_STEPS; ++i) {
                // 计算采样位置（朝向雷达中心）
                vec2 samplePos = uShipCoord + rayDir * travelDistance;

                // 边界检测（超出纹理范围停止）
                if (any(greaterThan(samplePos, vec2(1.0))) || 
                        any(lessThan(samplePos, vec2(0.0)))) break;

                // 采样高度并比较
                vec4 samplec = texture2D(uHeightMap, samplePos);
                float sampleHeight = (samplec.g*255.0+samplec.r)*255.0;
                if (sampleHeight > height) {
                    isOccluded = true;
                    break;
                }

                // 更新步进距离
                travelDistance += stepSize;
                if (travelDistance > rayLength) break; // 超过当前点距离时停止
            }

            // 输出结果
            if (isOccluded) {
                discard; // 或被遮挡时透明处理
            } else {
                // 示例：用高度值着色（实际应替换为雷达效果）
                // gl_FragColor = vec4(vec3(currentHeight), 1.0);
                if (height < uAntennaHeight)
                    discard;
                gl_FragColor = vec4(uDirectionalColor, 1.0);
            }

            /*
			vec4 c = texture2D(uHeightMap,vHeightTexCoord);
            float height = (c.g*255.0 + c.r)*255.0;
            if (height < uAntennaHeight)
                discard;
            gl_FragColor = vec4(uDirectionalColor, 1.0);
            */
		}
    }
 }
