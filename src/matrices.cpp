// Linear Algebra core of the whole project.
// Very important to make projections and transformations of the objects in the scene.
#include "../include/matrices.hpp"


// This function Matrix() helps in creating matrices using the GLM library.
//
// where the elements of the matrix are stored by traversing its COLUMNS.
// For example, let
//
//       [a b c]
//   M = [d e f]
//       [g h i]
//
// be a 3x3 matrix. In memory, in the "column-major" representation of OpenGL, this
// matrix is represented by the following array:
//
//   M[] = {  a,d,g,    b,e,h,    c,f,i  };
//              ^         ^         ^
//              |         |         |
//           column 1  column 2  column 3
//
// To allow us to define matrices through their ROWS, the Matrix() function
// computes the transpose using the elements passed as parameters.
void PrintVector(glm::vec4 v);


glm::mat4 Matrix(
    float m00, float m01, float m02, float m03, // ROW 1
    float m10, float m11, float m12, float m13, // ROW 2
    float m20, float m21, float m22, float m23, // ROW 3
    float m30, float m31, float m32, float m33  // ROW 4
)
{
    return glm::mat4(
        m00, m10, m20, m30, // COLUMN 1
        m01, m11, m21, m31, // COLUMN 2
        m02, m12, m22, m32, // COLUMN 3
        m03, m13, m23, m33  // COLUMN 4
    );
}

// Identity matrix.
glm::mat4 Matrix_Identity()
{
    return Matrix(
        1.0f , 0.0f , 0.0f , 0.0f , // ROW 1
        0.0f , 1.0f , 0.0f , 0.0f , // ROW 2
        0.0f , 0.0f , 1.0f , 0.0f , // ROW 3
        0.0f , 0.0f , 0.0f , 1.0f   // ROW 4
    );
}

// Translation matrix T. Let p=[px,py,pz,pw] be a point and t=[tx,ty,tz,0] a
// vector in homogeneous coordinates, defined in a Cartesian coordinate system.
// Then, the matrix T is defined by the following equality:
//
//     T*p = p+t.
//
glm::mat4 Matrix_Translate(float tx, float ty, float tz)
{
    return Matrix(
        // FILL IN HERE THE TRANSLATION MATRIX (3D) IN HOMOGENEOUS COORDINATES
        // USING THE PARAMETERS tx, ty, and tz
        1.0f , 0.0f , 0.0f , tx ,  // ROW 1
        0.0f , 1.0f , 0.0f , ty ,  // ROW 2
        0.0f , 0.0f , 1.0f , tz ,  // ROW 3
        0.0f , 0.0f , 0.0f , 1.0f    // ROW 4
    );
}

// Scaling matrix S of a point relative to the origin of the coordinate system.
// Let p=[px,py,pz,pw] be a point in homogeneous coordinates.
// Then, the matrix S is defined by the following equality:
//
//     S*p = [sx*px, sy*py, sz*pz, pw].
//
glm::mat4 Matrix_Scale(float sx, float sy, float sz)
{
    return Matrix(
        // FILL IN HERE THE SCALING MATRIX (3D) IN HOMOGENEOUS COORDINATES
        // USING THE PARAMETERS sx, sy, and sz
        sx ,   0.0f , 0.0f , 0.0f ,  // ROW 1
        0.0f , sy ,   0.0f , 0.0f ,  // ROW 2
        0.0f , 0.0f , sz ,   0.0f ,  // ROW 3
        0.0f , 0.0f , 0.0f , 1.0f    // ROW 4
    );
}

// Rotation matrix R of a point relative to the origin of the coordinate system
// and around the X axis (first vector of the coordinate system basis).
// Let p=[px,py,pz,pw] be a point in homogeneous coordinates.
// Then, the matrix R is defined by the following equality:
//
//   R*p = [ px, c*py-s*pz, s*py+c*pz, pw ];
//
// where 'c' and 's' are the cosine and sine of the rotation angle, respectively.
glm::mat4 Matrix_Rotate_X(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return Matrix(
        // FILL IN HERE THE ROTATION MATRIX (3D) AROUND THE X AXIS IN HOMOGENEOUS
        // COORDINATES, USING THE PARAMETERS c and s
        1.0f , 0.0f , 0.0f , 0.0f ,  // ROW 1
        0.0f , c ,    -s ,   0.0f ,  // ROW 2
        0.0f , s ,     c ,   0.0f ,  // ROW 3
        0.0f , 0.0f , 0.0f , 1.0f    // ROW 4
    );
}

// Rotation matrix R of a point relative to the origin of the coordinate system
// and around the Y axis (second vector of the coordinate system basis).
// Let p=[px,py,pz,pw] be a point in homogeneous coordinates.
// Then, the matrix R is defined by the following equality:
//
//   R*p = [ c*px+s*pz, py, -s*px+c*pz, pw ];
//
// where 'c' and 's' are the cosine and sine of the rotation angle, respectively.
glm::mat4 Matrix_Rotate_Y(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return Matrix(
        // FILL IN HERE THE ROTATION MATRIX (3D) AROUND THE Y AXIS IN HOMOGENEOUS
        // COORDINATES, USING THE PARAMETERS c and s
        c ,    0.0f , s ,    0.0f ,  // ROW 1
        0.0f , 1.0f , 0.0f , 0.0f ,  // ROW 2
        -s ,   0.0f , c ,    0.0f ,  // ROW 3
        0.0f , 0.0f , 0.0f , 1.0f    // ROW 4
    );
}

// Rotation matrix R of a point relative to the origin of the coordinate system
// and around the Z axis (third vector of the coordinate system basis).
// Let p=[px,py,pz,pw] be a point in homogeneous coordinates.
// Then, the matrix R is defined by the following equality:
//
//   R*p = [ c*px-s*py, s*px+c*py, pz, pw ];
//
// where 'c' and 's' are the cosine and sine of the rotation angle, respectively.
glm::mat4 Matrix_Rotate_Z(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return Matrix(
        // FILL IN HERE THE ROTATION MATRIX (3D) AROUND THE Z AXIS IN HOMOGENEOUS
        // COORDINATES, USING THE PARAMETERS c and s
        c    , -s   , 0.0f , 0.0f ,  // ROW 1
        s    ,  c   , 0.0f , 0.0f ,  // ROW 2
        0.0f , 0.0f , 1.0f , 0.0f ,  // ROW 3
        0.0f , 0.0f , 0.0f , 1.0f    // ROW 4
    );
}

// Function that calculates the Euclidean norm of a vector whose coefficients are
// defined in any orthonormal basis.
float norm(glm::vec4 v)
{
    float vx = v.x;
    float vy = v.y;
    float vz = v.z;

    return sqrt( vx*vx + vy*vy + vz*vz);
}

// Rotation matrix R of a point relative to the origin of the coordinate system
// and around the axis defined by the vector 'axis'. This matrix can be defined
// by Rodrigues' formula. Remember that the vector that defines the rotation axis
// must be normalized!
glm::mat4 Matrix_Rotate(float angle, glm::vec4 axis)
{
    float c = cos(angle);
    float s = sin(angle);

    glm::vec4 v = axis / norm(axis);

    float vx = v.x;
    float vy = v.y;
    float vz = v.z;
    float vx2 = vx*vx;
    float vy2 = vy*vy;
    float vz2 = vz*vz;
    float vxvy = vx*vy;
    float vxvz = vx*vz;
    float vyvz = vy*vz;
    float c1 = 1.0f - c;

    return Matrix(
        // FILL IN HERE THE ROTATION MATRIX (3D) AROUND THE AXIS v IN HOMOGENEOUS
        // COORDINATES, USING THE PARAMETERS vx, vy, vz, c, and s (RODRIGUES' FORMULA)
        vx2*c1 + c   ,   vxvy*c1 - vz*s , vxvz*c1+vy*s , 0.0f ,  // ROW 1
        vxvy*c1+vz*s ,   vy2*c1+c       , vyvz*c1-vx*s , 0.0f ,  // ROW 2
        vxvz*c1-vy*s ,   vyvz*c1+vx*s   , vz2*c1+c     , 0.0f ,  // ROW 3
        0.0f         ,   0.0f           , 0.0f         , 1.0f    // ROW 4
    );
}

// Cross product between two vectors u and v defined in an orthonormal coordinate system.
glm::vec4 crossproduct(glm::vec4 u, glm::vec4 v)
{
    float u1 = u.x;
    float u2 = u.y;
    float u3 = u.z;
    float v1 = v.x;
    float v2 = v.y;
    float v3 = v.z;

    return glm::vec4(
        u2*v3 - u3*v2, // First coefficient
        u3*v1 - u1*v3, // Second coefficient
        u1*v2 - u2*v1, // Third coefficient
        0.0f // w = 0 for vectors.
    );
}

// Dot product between two vectors u and v defined in an orthonormal coordinate system.
float dotproduct(glm::vec4 u, glm::vec4 v)
{
    float u1 = u.x;
    float u2 = u.y;
    float u3 = u.z;
    float u4 = u.w;
    float v1 = v.x;
    float v2 = v.y;
    float v3 = v.z;
    float v4 = v.w;

    if ( u4 != 0.0f || v4 != 0.0f )
    {
        fprintf(stderr, "ERROR: Dot product not defined for points.\n");
        std::exit(EXIT_FAILURE);
    }

    return u1*v1 + u2*v2 + u3*v3 + u4*v4;
}

// Coordinate change matrix to the Camera coordinate system.
glm::mat4 Matrix_Camera_View(glm::vec4 position_c, glm::vec4 view_vector, glm::vec4 up_vector)
{
    
    glm::vec4 w = -view_vector;   
    glm::vec4 u = crossproduct(up_vector,w);
    // Normalize the vectors u and w
    float view_vector_size = norm(view_vector);
    float u_size = norm(u);
    if(view_vector_size)
        w = w / view_vector_size; // Normalize vector w
    if(u_size)
        u = u / u_size; // Normalize vector u

    glm::vec4 v = crossproduct(w,u);
    glm::vec4 origin_o = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    float ux = u.x;
    float uy = u.y;
    float uz = u.z;
    float vx = v.x;
    float vy = v.y;
    float vz = v.z;
    float wx = w.x;
    float wy = w.y;
    float wz = w.z;

    glm::vec4 c = position_c - origin_o;


    return Matrix(
        // FILL IN HERE THE COORDINATE SYSTEM CHANGE MATRIX (3D)
        // TO CAMERA COORDINATES (HOMOGENEOUS VIEW MATRIX), USING
        // THE PARAMETERS ux,uy,uz, vx,vy,vz, wx,wy,wz, position_c, origin_o,
        // and the dotproduct() function.
        //
        // ATTENTION: The dot product, computed by the dotproduct() function, is
        // only defined for arguments that are VECTORS. There is no dot product
        // for POINTS.
        //
        ux ,   uy ,   uz ,   dotproduct(-u, c) ,  // ROW 1
        vx ,   vy ,   vz ,   dotproduct(-v, c) ,  // ROW 2
        wx ,   wy ,   wz ,   dotproduct(-w, c) ,  // ROW 3
        0.0f , 0.0f , 0.0f , 1.0f                 // ROW 4
    );
}

// Orthographic parallel projection matrix
glm::mat4 Matrix_Orthographic(float l, float r, float b, float t, float n, float f)
{
    glm::mat4 M = Matrix(
        // FILL IN HERE THE ORTHOGRAPHIC PROJECTION MATRIX M (3D) USING THE
        // PARAMETERS l,r,b,t,n,f 
        2/(r-l) , 0.0f ,    0.0f ,    -(r+l)/(r-l) ,  // ROW 1
        0.0f ,    2/(t-b) , 0.0f ,    -(t+b)/(t-b) ,  // ROW 2
        0.0f ,    0.0f ,    2/(f-n) , -(f+n)/(f-n) ,  // ROW 3
        0.0f ,    0.0f ,    0.0f ,     1.0f    // ROW 4
    );

    return M;
}

// Perspective projection matrix
glm::mat4 Matrix_Perspective(float field_of_view, float aspect, float n, float f)
{
    float t = fabs(n) * tanf(field_of_view / 2.0f);
    float b = -t;
    float r = t * aspect;
    float l = -r;
    glm::mat4 P = Matrix(
        // FILL IN HERE THE PERSPECTIVE PROJECTION MATRIX P (3D) USING THE
        // PARAMETERS n and f.
        n ,    0.0f ,    0.0f ,   0.0f ,  // ROW 1
        0.0f , n ,       0.0f ,   0.0f ,  // ROW 2
        0.0f , 0.0f ,    n+f ,    -f*n ,  // ROW 3
        0.0f , 0.0f ,    1.0f ,   0.0f    // ROW 4
    );

    // The matrix M is the same as computed above in Matrix_Orthographic().
    glm::mat4 M = Matrix_Orthographic(l, r, b, t, n, f);

    // Note that the matrices M*P and -M*P perform exactly the same perspective
    // projection, since the negative sign will not affect the result
    // due to the division by w. For example, let q = [qx,qy,qz,1] be a point:
    //
    //      M*P*q = [ qx', qy', qz', w ]
    //   =(div w)=> [ qx'/w, qy'/w, qz'/w, 1 ]   Eq. (*)
    //
    // now with the negative sign:
    //
    //     -M*P*q = [ -qx', -qy', -qz', -w ]
    //   =(div w)=> [ -qx'/-w, -qy'/-w, -qz'/-w, -w/-w ]
    //            = [ qx'/w, qy'/w, qz'/w, 1 ]   Eq. (**)
    //
    // Note that the final point, after division by w, is the same: Eq. (*) == Eq. (**).
    //
    // So, why do we use -M*P instead of M*P? Because the OpenGL specification
    // defines that points outside the unit cube NDC should be discarded since
    // they will not appear on the screen. The test that defines if a point
    // q is inside the unit cube NDC can be expressed as:
    //
    //      -1 <= qx'/w <= 1   &&  -1 <= qy'/w <= 1   &&  -1 <= qz'/w <= 1
    //
    // or, equivalently IF w > 0, the video card does the following test
    // BEFORE the division by w:
    //
    //      -w <= qx' <= w   &&  -w <= qy' <= w   &&  -w <= qz' <= w
    //
    // Note that the above test saves a division by w if the point is
    // discarded (when it is outside NDC), however, this last test is only
    // equivalent to the first test IF AND ONLY IF w > 0 (that is, if w is
    // positive). Since this last test is what the video card (GPU) will do,
    // we need to use the matrix -M*P for perspective projection, so that
    // w is positive.
    //
    return -M*P;
}

// Function that prints a matrix M to the terminal
void PrintMatrix(glm::mat4 M)
{
    printf("\n");
    printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ]\n", M[0][0], M[1][0], M[2][0], M[3][0]);
    printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ]\n", M[0][1], M[1][1], M[2][1], M[3][1]);
    printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ]\n", M[0][2], M[1][2], M[2][2], M[3][2]);
    printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ]\n", M[0][3], M[1][3], M[2][3], M[3][3]);
}

// Function that prints a vector v to the terminal
void PrintVector(glm::vec4 v)
{
    printf("\n");
    printf("[ %+0.2f ]\n", v[0]);
    printf("[ %+0.2f ]\n", v[1]);
    printf("[ %+0.2f ]\n", v[2]);
    printf("[ %+0.2f ]\n", v[3]);
}

// Function that prints the product of a matrix by a vector to the terminal
void PrintMatrixVectorProduct(glm::mat4 M, glm::vec4 v)
{
    auto r = M*v;
    printf("\n");
    printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ][ %+0.2f ]   [ %+0.2f ]\n", M[0][0], M[1][0], M[2][0], M[3][0], v[0], r[0]);
    printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ][ %+0.2f ] = [ %+0.2f ]\n", M[0][1], M[1][1], M[2][1], M[3][1], v[1], r[1]);
    printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ][ %+0.2f ]   [ %+0.2f ]\n", M[0][2], M[1][2], M[2][2], M[3][2], v[2], r[2]);
    printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ][ %+0.2f ]   [ %+0.2f ]\n", M[0][3], M[1][3], M[2][3], M[3][3], v[3], r[3]);
}

// Function that prints the product of a matrix by a vector, along with division
// by w, to the terminal.
void PrintMatrixVectorProductDivW(glm::mat4 M, glm::vec4 v)
{
    auto r = M*v;
    auto w = r[3];
    printf("\n");
    printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ][ %+0.2f ]   [ %+0.2f ]            [ %+0.2f ]\n", M[0][0], M[1][0], M[2][0], M[3][0], v[0], r[0], r[0]/w);
    printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ][ %+0.2f ] = [ %+0.2f ] =(div w)=> [ %+0.2f ]\n", M[0][1], M[1][1], M[2][1], M[3][1], v[1], r[1], r[1]/w);
    printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ][ %+0.2f ]   [ %+0.2f ]            [ %+0.2f ]\n", M[0][2], M[1][2], M[2][2], M[3][2], v[2], r[2], r[2]/w);
    printf("[ %+0.2f  %+0.2f  %+0.2f  %+0.2f ][ %+0.2f ]   [ %+0.2f ]            [ %+0.2f ]\n", M[0][3], M[1][3], M[2][3], M[3][3], v[3], r[3], r[3]/w);
}
