Membros: Nicolas Chaves - 316035


# Mini Golf 3D - Trabalho Final de Computação Gráfica

Projeto de um jogo 3D de minigolfe desenvolvido como trabalho final da disciplina de Computação Gráfica. Utiliza OpenGL moderno (GLFW + GLAD) e implementa os principais requisitos do trabalho: renderização com iluminação, malhas complexas, texturização, movimentação de câmera e física básica com detecção de colisões.

## 🎮 Funcionalidades

- Câmera interativa freecam e lookat
- Iluminação Phong
- Texturização com `stb_image`
- Detecção de colisão básica (esfera com malhas)
- Física simplificada inspirada na Unity
- Curvas de Bézier para trajetória inicial
- Malhas OBJ e malhas procedurais
- Separação entre código de renderização, lógica e física

---

## 🛠️ Compilação

Você pode compilar o projeto via `Makefile` ou `CMake`.

### 📁 Organização

```bash
.
├── include/          # Arquivos de cabeçalho (.h)
├── src/              # Código-fonte (.cpp e .c)
├── lib-linux/        # Bibliotecas estáticas para Linux (GLFW)
├── lib-mingw-64/     # Bibliotecas estáticas para Windows (MinGW 64)
├── build/obj/        # Objetos gerados (.o)
├── bin/Linux/        # Binários gerados (Linux)
├── bin/Windows/      # Binários gerados (Windows)
├── Makefile          # Sistema de build com g++
├── CMakeLists.txt    # Sistema de build alternativo com CMake
└── README.md
