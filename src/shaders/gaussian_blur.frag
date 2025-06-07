#version 330 core

out vec4 FragColor;

uniform vec2 tex_size;

uniform sampler2D screenTexture;
uniform bool horizontal;

// How far from the center to take samples from the fragment you are currently on
const int radius = 6;
// Keep it between 1.0f and 2.0f (the higher this is the further the blur reaches)
float spreadBlur = 2.0f;
float weights[radius];

void main()
{
    vec2 uv = gl_FragCoord.xy / tex_size;
    // Calculate the weights using the Gaussian equation
    float x = 0.0f;
    float total = 0.0;
    for (int i = 0; i < radius; i++)
    {
        // Decides the distance between each sample on the Gaussian function
        if (spreadBlur <= 2.0f)
        x += 3.0f / radius;
        else
        x += 6.0f / radius;

        weights[i] = exp(-0.5f * pow(x / spreadBlur, 2.0f)) / (spreadBlur * sqrt(2 * 3.14159265f));
        total += weights[i];
    }

    vec2 tex_offset = 1.0f / tex_size;
    vec3 result = texture(screenTexture,uv).rgb * weights[0];

    if (length(result) < 0.001) {
        result = vec3(0.0); // treat nearly-black as black
    }



    // Calculate horizontal blur
    if(horizontal)
    {
        for(int i = 1; i < radius; i++)
        {
            // Take into account pixels to the right
            result += texture(screenTexture, uv + vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
            // Take into account pixels on the left
            result += texture(screenTexture, uv - vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
        }
    }
    // Calculate vertical blur
    else
    {
        for(int i = 1; i < radius; i++)
        {
            // Take into account pixels above
            result += texture(screenTexture, uv + vec2(0.0, tex_offset.y * i)).rgb * weights[i];
            // Take into account pixels below
            result += texture(screenTexture, uv - vec2(0.0, tex_offset.y * i)).rgb * weights[i];
        }
    }

    if (!all(greaterThanEqual(result, vec3(0.0))) || any(isnan(result)) || any(isinf(result))) {
        discard;
    } else {
        FragColor = vec4(result, 1.0);
    }
}