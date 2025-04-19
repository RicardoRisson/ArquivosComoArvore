#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

using namespace std;
namespace fs = filesystem;

/*
----------Compilação----------
g++ -std=c++17 main.cpp -o main
./main [diretorio_opcional]
*/

class FileNode {
public:
    string nome;
    string tipo;        // "arquivo" ou "pasta"
    long tamanho;       // em bytes (0 para pastas)
    vector<FileNode> filhos;

    FileNode(const string& nome, const string& tipo, long tamanho = 0)
        : nome(nome), tipo(tipo), tamanho(tamanho) {}

    void adicionarFilho(const FileNode& filho) {
        filhos.push_back(filho);
    }

    void mostrar(int nivel = 0) const {
        string indent(nivel * 2, ' ');
        cout << indent << "|- " << nome;
        if (tipo == "arquivo") {
            cout << " (" << tamanho << " bytes)";
        }
        cout << endl;

        for (const auto& filho : filhos) {
            filho.mostrar(nivel + 1);
        }
    }
};

// Função que carrega a árvore de diretórios REAL
FileNode carregarArvore(const fs::path& caminho) {
    // Cria o nó raiz (pasta atual)
    FileNode raiz(caminho.filename().string(), "pasta");

    // Percorre o diretório
    for (const auto& entry : fs::directory_iterator(caminho)) {
        // Ignora links simbólicos e dispositivos
        if (!entry.is_symlink() && !entry.is_block_file() && !entry.is_character_file()) {
            if (entry.is_directory()) {
                // Carrega subpastas recursivamente
                FileNode subpasta = carregarArvore(entry.path());
                raiz.adicionarFilho(subpasta);
            } else if (entry.is_regular_file()) {
                // Adiciona arquivo
                raiz.adicionarFilho(FileNode(
                    entry.path().filename().string(),
                    "arquivo",
                    entry.file_size()
                ));
            }
        }
    }
    return raiz;
}

int main(int argc, char* argv[]) {
    fs::path diretorio_base = (argc > 1) ? argv[1] : fs::current_path();
    cout << "Diretorio inicial: " << diretorio_base << "\n\n";

    // Carrega a árvore REAL do sistema de arquivos
    FileNode raiz = carregarArvore(diretorio_base);

    // Menu principal
    int opcao_usuario;
    do {
        cout << "\nOpcoes:\n";
        cout << "1. Exibir arvore\n";
        cout << "2. Exportar HTML\n";
        cout << "3. Pesquisar\n";
        cout << "4. Sair\n";
        cout << "Digite: ";
        cin >> opcao_usuario;

        switch (opcao_usuario) {
            case 1:
                cout << "\nEstrutura:\n";
                raiz.mostrar();
                break;
                
            case 2:
                cout << "\n(Em breve: exportar HTML)\n";
                break;
                
            case 3:
                cout << "\n(Em breve: pesquisar)\n";
                break;
                
            case 4:
                cout << "\nSaindo...\n";
                break;
                
            default:
                cout << "\nOpcao invalida!\n";
        }
    } while (opcao_usuario != 4);

    return 0;
}