#include <stdlib.h>
#include <stdio.h>
#include "math.h"

struct VERTICE_TEX_NOR
{
	  float x, y, z;
	  float r, g, b;
	  float s, t;
	  float nx,ny,nz;
};

struct VERTICE_XYZ
{
    	  float x, y, z;
};

float a = 0.4f;

static float radio = 1.0f;
// =============================================================================
//
// =============================================================================
float func_circ( float x )
{
    return sqrt(radio * radio - x * x);
}

float func_const( float x )
{
    return a;
}

float func_parab( float x )
{
    return x * x + 0.02;
}

float func_sin( float x )
{
    return  0.5 * abs(sin( 3.14159 * x )) + 0.02;
}
// =============================================================================
//                        creaResorte
// =============================================================================
VERTICE_TEX_NOR *creaResorte(float a, int M, int N_Vueltas)
{
    float menos_pi    = - 4.0 * atan(1.0);
    float deltaTheta  = ( 8.0 * atan(1.0) * N_Vueltas ) / ( M - 1 );
    float deltaPhi    =   4.0 * atan(1.0) / ( M - 1 );
    float theta, phi;

    float x,y,z;
    float r_yz;
    float norma;

    VERTICE_TEX_NOR* vertices = (VERTICE_TEX_NOR*) malloc( M * sizeof(VERTICE_TEX_NOR));

    int k;
    for( k = 0; k < M; k++)
    {
        theta = deltaTheta * k;
        phi   = menos_pi + deltaPhi * k;
        x     = a * cos(phi);
        r_yz  = a * sin(phi);
        y     = r_yz * cos(theta);
        z     = r_yz * sin(theta);

        //printf("%d %f %f %f \n",k,x,y,z);

        vertices[k].x = x;
        vertices[k].y = y;
        vertices[k].z = z;

        vertices[k].r = 1.0; //((double) rand() / (RAND_MAX+1));
        vertices[k].g = 1.0; //((double) rand() / (RAND_MAX+1));
        vertices[k].b = 1.0; //((double) rand() / (RAND_MAX+1));

        norma = sqrt(y * y + z * z);
        if( norma == 0.f) norma = 1.0;

        vertices[k].nx = 0.f;
        vertices[k].ny = y / norma;
        vertices[k].nz = z / norma;

    }

    return vertices;
}
// =============================================================================
//             ----------- creaSolRev -----------
// =============================================================================
VERTICE_TEX_NOR *creaSolRev(float (*func)(float),float radio, float x_min, float x_max, int M_PuntosPorRodaja,int N_Rodajas)
{
    // son M_PuntosPorRodaja - 1 fascetas
    // con M_PuntosPorRodaja y el ultimo coincide con el primero( indices 0 y M_PuntosPorRodaja - 1 )
    //

    float deltaTheta  = 8.0 * atan(1.0) / ( M_PuntosPorRodaja - 1 );
    float deltaX = ( x_max - x_min ) / ( N_Rodajas -1 );
    float theta;
    float x,y,z;
    float r_yz;
    float norma;

    //float radio2 = radio * radio;

    VERTICE_TEX_NOR* vertices = (VERTICE_TEX_NOR*) malloc( ( 2 + N_Rodajas * M_PuntosPorRodaja ) * sizeof(VERTICE_TEX_NOR));

    int k_rod, k_t, k_indice;

    for( k_rod = 0; k_rod < N_Rodajas; k_rod++)
    {
        x     = x_min + k_rod * deltaX;
        r_yz  = (*func)(x);//sqrt( radio2 - x * x);

        k_indice = k_rod * M_PuntosPorRodaja;

        for(k_t = 0; k_t < M_PuntosPorRodaja; k_t++)
        {
          theta = k_t * deltaTheta;
          y = r_yz * cos(theta);
          z = r_yz * sin(theta);

          //printf("%d %f %f %f \n",k_indice+k_t,x,y,z);

          vertices[k_indice + k_t].x = x;
          vertices[k_indice + k_t].y = y;
          vertices[k_indice + k_t].z = z;

          vertices[k_indice + k_t].s = ((float) k_t ) / ((float) M_PuntosPorRodaja);
          vertices[k_indice + k_t].t = ((float)k_rod) / ((float)N_Rodajas); //k_rod % 2 == 0 ? 1.f : 0.f;

          vertices[k_indice + k_t].r = ( k_rod % 3 ) == 0 ? 1.0f : 0; //(float)((double) rand() / (RAND_MAX+1));
          vertices[k_indice + k_t].g = ( k_rod % 3 ) == 1 ? 1.0f : 0; //(float)((double) rand() / (RAND_MAX+1));
          vertices[k_indice + k_t].b = ( k_rod % 3 ) == 2 ? 1.0f : 0; //(float)((double) rand() / (RAND_MAX+1));

          norma = sqrt(y * y + z * z);
          norma = norma == 0.0 ? 1.0 : norma;

          vertices[k_indice + k_t].nx = 0;
          vertices[k_indice + k_t].ny = y / norma;
          vertices[k_indice + k_t].nz = z / norma;

        }

        vertices[k_indice + k_t]     = {-radio,0.0f,0.0f,1.0f,1.0f,1.0f, 0.5f, 0.5f, -1.f, 0.f, 0.f };
        vertices[k_indice + k_t + 1] = { radio,0.0f,0.0f,0.0f,0.0f,0.0f, 0.5f, 0.5f,  1.f, 0.f, 0.f };
    }
    return vertices;
}
// =============================================================================
//                    creaTriangSolRev
// =============================================================================
VERTICE_TEX_NOR *creaTriangSolRev(float (*func)(float),float radio, float x_min, float x_max, int M_PuntosPorRodaja,int N_Rodajas, int *num_vertices, int *num_triangulos)
{
    int caras               = (N_Rodajas-1) * ( M_PuntosPorRodaja - 1);
    int caras_tapa          =  M_PuntosPorRodaja - 1;
    *num_triangulos         = 2 * caras + 2 * caras_tapa;
    int num_vertices_base   = N_Rodajas * M_PuntosPorRodaja + 2;
    int num_vertices_triang = 3 * (*num_triangulos);
    *num_vertices           = num_vertices_triang;
    //int vertices_caras_lat  = M_PuntosPorRodaja * N_Rodajas;

    int k_rod, k_cara;
    int k_ver_rod;     // número de vertices antes de la rodaja
    int i_ver_rod;     // vertice en la rodaja
    int k_ver_triang;  // indice del vertice en la colección de vertices de los triangulos
    //int k_vertice_tapa;

    VERTICE_TEX_NOR* vertices_base   = creaSolRev(func, radio, x_min, x_max, M_PuntosPorRodaja, N_Rodajas);

    VERTICE_TEX_NOR* vertices_triang = (VERTICE_TEX_NOR*) malloc( num_vertices_triang * sizeof(VERTICE_TEX_NOR));

    k_ver_triang = 0;

    for(k_rod = 0; k_rod < N_Rodajas - 1; k_rod++)
    {
        k_ver_rod = k_rod * M_PuntosPorRodaja;
        for( i_ver_rod = 0; i_ver_rod < M_PuntosPorRodaja - 1; i_ver_rod++)
        {
            vertices_triang[k_ver_triang++] = vertices_base[k_ver_rod                     + i_ver_rod    ];
            vertices_triang[k_ver_triang++] = vertices_base[k_ver_rod + M_PuntosPorRodaja + i_ver_rod    ];
            vertices_triang[k_ver_triang++] = vertices_base[k_ver_rod + M_PuntosPorRodaja + i_ver_rod + 1];

            vertices_triang[k_ver_triang++] = vertices_base[k_ver_rod                     + i_ver_rod    ];
            vertices_triang[k_ver_triang++] = vertices_base[k_ver_rod + M_PuntosPorRodaja + i_ver_rod + 1];
            vertices_triang[k_ver_triang++] = vertices_base[k_ver_rod                     + i_ver_rod + 1];
        }
    }

    for(k_cara = 0; k_cara < M_PuntosPorRodaja - 1; k_cara++)
    {
            vertices_triang[k_ver_triang++] = vertices_base[num_vertices_base - 2             ];
            vertices_triang[k_ver_triang++] = vertices_base[k_cara                            ];
            vertices_triang[k_ver_triang++] = vertices_base[k_cara + 1                        ];

            vertices_triang[k_ver_triang++] = vertices_base[num_vertices_base - 1             ];
            vertices_triang[k_ver_triang++] = vertices_base[num_vertices_base - 3 - k_cara    ];
            vertices_triang[k_ver_triang++] = vertices_base[num_vertices_base - 3 - k_cara - 1];

    }

    //*num_vertices = M_PuntosPorRodaja * N_Rodajas;
    //return vertices_base;

    return vertices_triang;

}

// =====================================================================================================
//   Minirutina alambrada para construir el tetraedro con los índices de las caras
//   Notense los ** en los nombres de las variables para asignar los arreglos por medio del malloc()
// =====================================================================================================

void creaSolCaras(int *num_vertices, VERTICE_TEX_NOR   **lug_ap_vertices, // lugar de la memoria del apuntador a VERTICES
                  int *num_indices,  unsigned int      **lug_ap_ui_indices) // lugar de la memoria del apuntador a unsigned ints
{
   VERTICE_TEX_NOR vertices_punto[4] = {
    {  a,  a,  a, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f },
	{ -a,  a, -a, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f },
	{ -a, -a,  a, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f },
	{  a, -a, -a, 1.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f }
    };

    *lug_ap_vertices = (VERTICE_TEX_NOR*) malloc(4*sizeof(VERTICE_TEX_NOR));
    int k;
    for(k = 0; k < 4; k++) (*lug_ap_vertices)[k] = vertices_punto[k];


    unsigned int indices_punto[12] = { 0,1,2, // Cara I (CCW)
                                       0,2,3, // Cara II
                                       1,0,3, // Cara III
                                       1,3,2  // Cara IV
                                     };

    *lug_ap_ui_indices = (unsigned int*) malloc(12 * sizeof(unsigned int));
    for( int k = 0; k < 12 ; k++) (*lug_ap_ui_indices)[k] = indices_punto[k];

    *num_vertices = 4;
    *num_indices  = 12;
}
// =============================================================================
//                     creaCaras
// =============================================================================
unsigned int * creaCaras(int M_PuntosPorRodaja,int N_Rodajas,int *num_indices)
{
    int caras_lat           = (N_Rodajas-1) * ( M_PuntosPorRodaja - 1);
    int caras_tapa          =  M_PuntosPorRodaja - 1;
    int num_triangulos      = 2 * caras_lat + 2 * caras_tapa;
    int num_vertices_base   = N_Rodajas * M_PuntosPorRodaja + 2;
    int num_vertices_triang = 3 * num_triangulos;

    *num_indices = num_vertices_triang;

    int k_ver_triang;
    int k_ver_rod;
    int k_cara;
    int k_rod;
    int i_ver_rod;

    unsigned int *caras = ( unsigned int*) malloc( num_vertices_triang * sizeof(unsigned int));

    k_ver_triang = 0;

    for(k_rod = 0; k_rod < N_Rodajas - 1; k_rod++)
    {
        k_ver_rod = k_rod * M_PuntosPorRodaja;
        for( i_ver_rod = 0; i_ver_rod < M_PuntosPorRodaja - 1; i_ver_rod++)
        {
            caras[k_ver_triang++] = k_ver_rod                     + i_ver_rod    ;
            caras[k_ver_triang++] = k_ver_rod + M_PuntosPorRodaja + i_ver_rod    ;
            caras[k_ver_triang++] = k_ver_rod + M_PuntosPorRodaja + i_ver_rod + 1;

            caras[k_ver_triang++] = k_ver_rod                     + i_ver_rod    ;
            caras[k_ver_triang++] = k_ver_rod + M_PuntosPorRodaja + i_ver_rod + 1;
            caras[k_ver_triang++] = k_ver_rod                     + i_ver_rod + 1;
        }
    }

    for(k_cara = 0; k_cara < M_PuntosPorRodaja - 1; k_cara++)
    {
            caras[k_ver_triang++] = num_vertices_base - 2             ;
            caras[k_ver_triang++] = k_cara                            ;
            caras[k_ver_triang++] = k_cara + 1                        ;

            caras[k_ver_triang++] = num_vertices_base - 1             ;
            caras[k_ver_triang++] = num_vertices_base - 3 - k_cara    ;
            caras[k_ver_triang++] = num_vertices_base - 3 - k_cara - 1;

    }

    printf("creaCaras: *num_indices=%d, k_ver_triang=%d\n",*num_indices, k_ver_triang);

    return  caras;
}
// =============================================================================
//                          creaSolRevCaras
// =============================================================================
void creaSolRevCaras(int TIPO,
                     int *num_vertices, VERTICE_TEX_NOR  **lug_ap_vertices, // lugar de la memoria del apuntador a VERTICES
                     int *num_indices,  unsigned int     **lug_ap_ui_indices) // lugar de la memoria del apuntador a unsigned ints
{
    int M_puntos_rodaja;
    int N_rodajas;

    VERTICE_TEX_NOR *vertices;

    //VERTICE *vertices = creaResorte(a,M_resorte,N_vueltas_resorte); num_vertices = M_resorte;

    radio = 1.0f * a;

    switch( TIPO )
    {
        //  6,371 km  ->  1
        //  1,737.4 km -> 0.27
        //  0.001 ->  0.00000016

      case 1:  // LUNA
             radio=0.17f*a; //             radio=0.027f*a;
             M_puntos_rodaja = 100;
             N_rodajas       = 200;
             vertices = creaSolRev(func_circ,1.1* a, -a,a,M_puntos_rodaja,N_rodajas);
             break;
      case 2:  // TIERRA
             radio=0.45f*a;  //             radio=0.1f*a;
             M_puntos_rodaja = 100;
             N_rodajas       = 200;
             vertices = creaSolRev(func_circ,1.1* a, -a,a,M_puntos_rodaja,N_rodajas);
             break;
      case 3:  // CILINDRO
             M_puntos_rodaja = 21;
             N_rodajas       = 2;
             vertices = creaSolRev(func_const,1.1* a, -a,a,M_puntos_rodaja,N_rodajas);
            break;
      case 4:  // SATELITE
             radio=0.06f*a; //             radio=0.01f*a;
             M_puntos_rodaja = 100;
             N_rodajas       = 200;
             vertices = creaSolRev(func_circ,1.1* a, -a,a,M_puntos_rodaja,N_rodajas);
            break;
      default:  // CUBO
             M_puntos_rodaja = 5;
             N_rodajas       = 2;
             vertices = creaSolRev(func_const, a, -0.75*a, 0.75*a, M_puntos_rodaja,N_rodajas);
    }

    *lug_ap_vertices = vertices;

    *num_vertices = M_puntos_rodaja * N_rodajas + 2;

    unsigned int * indices = creaCaras(M_puntos_rodaja,N_rodajas,num_indices);

    *lug_ap_ui_indices = indices;

    printf("creaSolRevCaras: *num_vertices=%d, *num_indices=%d\n",*num_vertices,*num_indices);

}

VERTICE_TEX_NOR *ejes(int * num_vertices)
{
  /*
  VERTICE_TEX vertices_ejes[6] = {
    {  0.f,  0.f,  0.f, 1.f, 0.f, 0.f, 0.f, 0.f },
	{  1.f,  0.f,  0.f, 1.f, 0.f, 0.f, 0.f, 0.f },
    {  0.f,  0.f,  0.f, 0.f, 1.f, 0.f, 0.f, 0.f },
	{  0.f,  1.f,  0.f, 0.f, 1.f, 0.f, 0.f, 0.f },
    {  0.f,  0.f,  0.f, 0.f, 0.f, 1.f, 0.f, 0.f },
	{  0.f,  0.f,  1.f, 0.f, 0.f, 1.f, 0.f, 0.f },
    };
  */
  VERTICE_TEX_NOR vertices_ejes[6] = {
    { 0.0f,  0.0f,  -6.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.5774f, 0.5774f, 0.5774f },
	{ 1.0f,  0.0f,  -6.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.0f,    0.7071f, 0.7071f },
    { 0.0f,  0.0f,  -6.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.5774f, 0.5774f, 0.5774f },
	{ 0.0f,  1.0f,  -6.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.7071f, 0.0f,    0.7071f },
    { 0.0f,  0.0f,  -6.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.5774f, 0.5774f, 0.5774f },
	{ 0.0f,  0.0f,  -5.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.7071f, 0.7071f, 0.0f    }
    };


    VERTICE_TEX_NOR *vertices = (VERTICE_TEX_NOR*) malloc(6*sizeof(VERTICE_TEX_NOR));
    int k;
    for(k = 0; k < 6; k++) vertices[k] = vertices_ejes[k];

    *num_vertices = 6;

    return vertices;
}
// =============================================================================
//
// =============================================================================
