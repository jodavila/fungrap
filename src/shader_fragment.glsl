#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 cor_interpolada_pelo_rasterizador;
in vec4 position_world;
in vec4 normal;


// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


// Identificador que define qual objeto está sendo desenhado no momento
#define BALL   0
#define CLUB  1
#define WALL  2
#define FLOOR 3
#define VEGETATION 4
#define CLOUDS 5
uniform int object_id;
in vec2 TexCoords;              // <--- já vem do vertex shader
uniform sampler2D texture_diffuse; // <--- nova uniform
uniform bool use_texture; // nova flag para ativar ou não textura


// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(0.8,1.0,0.5,0.0));

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = reflect(-l, n);

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong

    if ( object_id == BALL )
    {
        vec3 Kd_base = vec3(0.9, 0.9, 0.9);
        if (use_texture)
            Kd = texture(texture_diffuse, TexCoords).rgb * Kd_base*2;
        else
            Kd = Kd_base;

        Ks = vec3(0.9,0.9,0.9);
        Ka = vec3(0.4,0.4,0.4);
        q = 100.0;
    }
    else if ( object_id == CLUB )
    {
        vec3 Kd_base = vec3(0.08, 0.4, 0.8);
        if (use_texture)
            Kd = texture(texture_diffuse, TexCoords).rgb * Kd_base;
        else
            Kd = Kd_base;
        Ks = vec3(0.8,0.8,0.8);
        Ka = vec3(0.4,0.2,0.4);
        q = 32.0;
    }
    else if ( object_id == FLOOR )
    {
        vec3 Kd_base = vec3(0.001,0.9,0.2);
        if (use_texture)
            Kd = texture(texture_diffuse, TexCoords).rgb * Kd_base;
        else
            Kd = Kd_base;
        Ks = vec3(0.3,0.3,0.3);
        Ka = vec3(0.2,0.5,0.3);
        q = 100.0;
    }
    else if ( object_id == WALL )
    {
        vec3 Kd_base = vec3(0.0,0.0,0.8);
        if (use_texture)
            Kd = texture(texture_diffuse, TexCoords).rgb * Kd_base;
        else
            Kd = Kd_base;
        Ks = vec3(0.2,0.2,0.2);
        Ka = vec3(0.8,0.8,0.8);
        q = 1.0;
    }
    else if ( object_id == VEGETATION )
    {
        vec3 Kd_base = vec3(0.1, 0.6, 0.1);
        if (use_texture)
            Kd = texture(texture_diffuse, TexCoords).rgb * Kd_base;
        else
            Kd = Kd_base;
        Ks = vec3(0.2, 0.2, 0.2);
        Ka = vec3(0.004, 0.004, 0.004);
        q = 10.0;
    }
    else if ( object_id == CLOUDS)
    {
        vec3 Kd_base = vec3(1.0, 1.0, 1.0);
        if (use_texture)
            Kd = texture(texture_diffuse, TexCoords).rgb * Kd_base;
        else
            Kd = Kd_base;
        Ks = vec3(0.5, 0.5, 0.5);
        Ka = vec3(0.8, 0.8, 0.8);
        q = 1000.0;
    }
    
    
    
    else // Objeto desconhecido = preto
    {
       color = cor_interpolada_pelo_rasterizador; // Cor do objeto
       return;
    }

    // Espectro da fonte de iluminação
    vec3 I = vec3(1.0,1.0,1.0);

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.5,0.5,0.5); 

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term = I * Kd * max(dot(n.xyz, l.xyz), 0.0);

    // Termo ambiente
    vec3 ambient_term = Ia * Ka;
    // Termo especular utilizando o modelo de iluminação de Phong
    
    vec3 phong_specular_term = I * Ks * pow(max(dot(r.xyz, v.xyz), 0.0), q);


    // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
    // necessário:
    // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
    //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
    //      glEnable(GL_BLEND);
    //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
    //    todos os objetos opacos; e
    // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
    //    suas distâncias para a câmera (desenhando primeiro objetos
    //    transparentes que estão mais longe da câmera).
    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1;

    // Cor final do fragmento calculada com uma combinação dos termos difuso,
    // especular, e ambiente. Veja slide 129 do documento Aula_17_e_18_Modelos_de_Iluminacao.pdf.
    color.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;

    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
    
} 
