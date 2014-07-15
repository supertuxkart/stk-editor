#ifndef SHADER_HPP
#define SHADER_HPP

const char vs[] =
"varying vec3 normal;                           "
"void main()                                    "
"{                                              "
"    gl_TexCoord[0] = gl_MultiTexCoord0;        "
"    gl_TexCoord[1] = gl_MultiTexCoord1;        "
"    gl_Position = ftransform();                "
"                                               "
"    normal = normalize(gl_Normal);             "
"}                                              ";

const char ps[] =
"uniform sampler2D splatting;                                "
"uniform sampler2D terrain_1;                                "
"uniform sampler2D terrain_2;                                "
"uniform sampler2D terrain_3;                                "
"uniform sampler2D terrain_4;                                "
"                                                            "
"varying vec3 normal;                                        "
"                                                            "
"void main()                                                 "
"{                                                           "
"    vec4 splat = texture2D(splatting, gl_TexCoord[1].st);   "
"    vec4 t1 = texture2D(terrain_1, gl_TexCoord[0].st);      "
"    vec4 t2 = texture2D(terrain_2, gl_TexCoord[0].st);      "
"    vec4 t3 = texture2D(terrain_3, gl_TexCoord[0].st);      "
"    vec4 t4 = texture2D(terrain_4, gl_TexCoord[0].st);      "
"                                                            "
"    gl_FragColor = (splat.r * t1 +                          "
"        splat.g * t2 +                                      "
"        splat.b * t3 +                                      "
"        (1.0 - splat.r - splat.g - splat.b) * t4 )          "
"        * min(1.0, 0.1 + dot(vec3(0, 1, 0), normal));       "
"}                                                           ";


#endif

