#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>

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

    // Retorna o número de filhos diretos (não recursivo)
    int contarFilhos() const {
        return filhos.size();
    }

    // Calcula o tamanho total recursivamente
    long calcularTamanhoTotal() const {
        if (tipo == "arquivo") {
            return tamanho;
        }
        
        long tamanho_total = 0;
        for (const auto& filho : filhos) {
            tamanho_total += filho.calcularTamanhoTotal();
        }
        return tamanho_total;
    }

    void mostrar(int nivel = 0) const {
        string indent(nivel * 2, ' ');
        cout << indent << "|- " << nome;
        if (tipo == "arquivo") {
            cout << " (" << tamanho << " bytes)";
        } else {
            // Mostra número de filhos e tamanho total para pastas
            int num_filhos = contarFilhos();
            long tamanho_total = calcularTamanhoTotal();
            cout << " (" << num_filhos << (num_filhos == 1 ? " filho" : " filhos")
                 << ", " << tamanho_total << " bytes)";
        }
        cout << endl;

        for (const auto& filho : filhos) {
            filho.mostrar(nivel + 1);
        }
    }

    string gerarHTML(int nivel = 0) const {
        string indent(nivel * 4, ' ');
        string html = indent + "<li>\n";
        html += indent + "    <span class='" + tipo + "'>" + nome;
        
        if (tipo == "arquivo") {
            html += " (" + to_string(tamanho) + " bytes)";
        } else {
            int num_filhos = contarFilhos();
            long tamanho_total = calcularTamanhoTotal();
            html += " (" + to_string(num_filhos) + (num_filhos == 1 ? " filho" : " filhos");
            html += ", " + to_string(tamanho_total) + " bytes)";
        }
        html += "</span>\n";

        if (!filhos.empty()) {
            html += indent + "    <ul>\n";
            for (const auto& filho : filhos) {
                html += filho.gerarHTML(nivel + 1);
            }
            html += indent + "    </ul>\n";
        }
        
        html += indent + "</li>\n";
        return html;
    }
};

// Função para exportar a árvore de diretórios para um arquivo HTML
void exportarHTML(const FileNode& raiz, const string& caminho) {
    ofstream arquivo(caminho);
    if (!arquivo.is_open()) {
        cout << "Erro ao criar arquivo HTML!" << endl;
        return;
    }

    arquivo << "<!DOCTYPE html>\n"
            << "<html>\n<head>\n"
            << "    <meta charset='UTF-8'>\n"
            << "    <title>Árvore de Arquivos</title>\n"
            << "    <style>\n"
            << "        .arquivo { color: blue; }\n"
            << "        .pasta { color: brown; font-weight: bold; }\n"
            << "        ul { list-style-type: none; }\n"
            << "    </style>\n"
            << "</head>\n<body>\n"
            << "    <h1>Estrutura de Arquivos</h1>\n"
            << "    <ul>\n"
            << raiz.gerarHTML()
            << "    </ul>\n"
            << "</body>\n</html>";

    arquivo.close();
    cout << "Arquivo HTML gerado com sucesso: " << caminho << endl;
}

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
                
            case 2: {
                string arquivo_saida = "arvore.html";
                cout << "\nExportando para HTML...\n";
                exportarHTML(raiz, arquivo_saida);
                break;
            }
                
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