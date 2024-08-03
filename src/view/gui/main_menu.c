#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>
#include <stdlib.h>

// Callback para erro GLFW
static void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

int main() {
    // Inicializa GLFW
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        return 1;
    }

    // Cria uma janela OpenGL
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Menu GUI", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Inicializa GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return 1;
    }

    // Configura ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        // Inicia frame ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Configura o menu
        ImGui::Begin("Menu Principal");
        if (ImGui::Button("Cadastrar Cliente")) {
            // Adicione seu código para cadastrar cliente aqui
        }
        if (ImGui::Button("Buscar Cliente")) {
            // Adicione seu código para buscar cliente aqui
        }
        if (ImGui::Button("Cadastrar Funcionário")) {
            // Adicione seu código para cadastrar funcionário aqui
        }
        if (ImGui::Button("Buscar Funcionário")) {
            // Adicione seu código para buscar funcionário aqui
        }
        if (ImGui::Button("Cadastrar Exercício")) {
            // Adicione seu código para cadastrar exercício aqui
        }
        if (ImGui::Button("Buscar Exercício")) {
            // Adicione seu código para buscar exercício aqui
        }
        if (ImGui::Button("Cadastrar Treino")) {
            // Adicione seu código para cadastrar treino aqui
        }
        if (ImGui::Button("Buscar Treino")) {
            // Adicione seu código para buscar treino aqui
        }
        if (ImGui::Button("Sair")) {
            break;
        }
        ImGui::End();

        // Renderiza
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpeza
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
