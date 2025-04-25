#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

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
    string caminho_completo;

    // Construtor: Inicializa um nó da árvore com nome, tipo ("arquivo" ou "pasta") e tamanho
    // @param nome: Nome do arquivo ou pasta
    // @param tipo: Tipo do nó ("arquivo" ou "pasta")
    // @param tamanho: Tamanho em bytes (0 por padrão, usado principalmente para arquivos)
    FileNode(const string& nome, const string& tipo, long tamanho = 0)
        : nome(nome), tipo(tipo), tamanho(tamanho), caminho_completo(caminho_completo) {}

    // Adiciona um nó filho (arquivo ou pasta) ao nó atual
    // @param filho: Nó a ser adicionado como filho
    void adicionarFilho(const FileNode& filho) {
        filhos.push_back(filho);
    }

    // Retorna o número de filhos diretos (não recursivo) do nó atual
    // @return O número total de nós filhos imediatos (arquivos e pastas)
    // @note Este método conta apenas os filhos diretos, não incluindo subpastas
    //       ou arquivos dentro das subpastas. Para uma contagem completa de
    //       todos os itens na árvore, seria necessário um método recursivo.
    // @see calcularTamanhoTotal() para um exemplo de método recursivo
    int contarFilhos() const {
        return filhos.size();
    }

    // Calcula o tamanho total de um nó e seus descendentes recursivamente
    // @return O tamanho total em bytes:
    //         - Para arquivos: retorna o tamanho do próprio arquivo
    //         - Para pastas: soma o tamanho de todos os arquivos contidos nela e suas subpastas
    // @note Este método é recursivo e percorre toda a árvore abaixo do nó atual
    // @example Para uma pasta com estrutura:
    //          /pasta (0 bytes)
    //          ├── arquivo1.txt (100 bytes)
    //          └── subpasta
    //              └── arquivo2.txt (200 bytes)
    //          O tamanho total será 300 bytes (100 + 200)
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

    // Exibe a estrutura de arquivos e diretórios em formato de árvore no console
    // @param nivel: Nível de profundidade do nó atual na árvore (0 para raiz)
    // @param ultimo: Indica se é o último item em seu nível (true = ├──, false = └──)
    // @note Este método usa caracteres especiais para criar a visualização hierárquica:
    //       │   : Linha vertical para conectar níveis
    //       ├── : Conexão para itens intermediários
    //       └── : Conexão para o último item
    // @example Exemplo de saída:
    //          pasta (2 filhos, 300 bytes)
    //          ├── arquivo.txt (100 bytes)
    //          └── subpasta (1 filho, 200 bytes)
    //              └── outro.txt (200 bytes)
    void mostrar(int nivel = 0, bool ultimo = true) const {
        // Indentação inicial
        if (nivel > 0) {
            for (int i = 0; i < nivel - 1; i++) {
                cout << "|   ";
            }
            cout << (ultimo ? "└── " : "├── ");
        }

        // Nome e informações
        cout << nome;
        if (tipo == "arquivo") {
            cout << " (" << tamanho << " bytes)";
        } else {
            int num_filhos = contarFilhos();
            long tamanho_total = calcularTamanhoTotal();
            cout << " (" << num_filhos << (num_filhos == 1 ? " filho" : " filhos")
                 << ", " << tamanho_total << " bytes)";
        }
        cout << endl;

        // Mostra filhos com a linha vertical
        if (!filhos.empty()) {
            for (size_t i = 0; i < filhos.size(); i++) {
                filhos[i].mostrar(nivel + 1, i == filhos.size() - 1);
            }
        }
    }

    // Gera a representação HTML da árvore de arquivos e diretórios
    // @param nivel: Nível de profundidade do nó atual na árvore (0 para raiz)
    // @param ultimo: Indica se é o último item em seu nível (true para último item)
    // @return Uma string contendo o HTML formatado com indentação e estilos
    // @note Este método gera uma representação visual da árvore usando caracteres especiais:
    //       │   : Linha vertical para conectar níveis
    //       ├── : Conexão para itens intermediários
    //       └── : Conexão para o último item
    // @example Exemplo de saída HTML:
    //          <span class='pasta'>documentos (2 filhos, 300 bytes)</span>
    //          ├── <span class='arquivo'>relatorio.txt (100 bytes)</span>
    //          └── <span class='pasta'>imagens (1 filho, 200 bytes)</span>
    string gerarHTML(int nivel = 0, bool ultimo = true) const {

        string html;
        
        // Indentação inicial
        for (int i = 0; i < nivel; i++) {
            html += (i == nivel - 1 ? (ultimo ? "└── " : "├── ") : "│   ");
        }
        
        // Nome e informações
        html += "<span class='" + tipo + "'>" + nome;
        if (tipo == "arquivo") {
            html += " (" + to_string(tamanho) + " bytes)";
        } else {
            int num_filhos = contarFilhos();
            long tamanho_total = calcularTamanhoTotal();
            html += " (" + to_string(num_filhos) + (num_filhos == 1 ? " filho" : " filhos")
                   + ", " + to_string(tamanho_total) + " bytes)";
        }
        html += "</span><br>\n";

        // Adiciona filhos
        if (!filhos.empty()) {
            for (size_t i = 0; i < filhos.size(); i++) {
                html += filhos[i].gerarHTML(nivel + 1, i == filhos.size() - 1);
            }
        }
        
        return html;
    }
    void encontraMaiorArquivo(long& max_tam, vector<string>& caminhos) const {
        if (tipo == "arquivo") {                        // se o nó for arquivo
            if (tamanho > max_tam) {                    // e se for maior que o tamanho atual máximo
                max_tam = tamanho;                      // atualiza o maior tamanho
                caminhos.clear();                       // limpa os caminhos anteriores
                caminhos.push_back(caminho_completo);   // adiciona o novo maior
            } else if (tamanho == max_tam) {
                caminhos.push_back(caminho_completo);   // se for do mesmo tamanho do maior, adiciona também
            }
        }
        for (const auto& filho : filhos) {              // aplica recursivamente para todos os filhos (arquivos e subpastas)
            filho.encontraMaiorArquivo(max_tam, caminhos);
        }
    }

    void buscaPorExtensao(const string& ext, vector<string>& arquivos) const {
        if (tipo == "arquivo" && nome.size() >= ext.size() && nome.substr(nome.size() - ext.size()) == ext) { // verifica se o nó é um arquivo e se o nome termina com a extensão desejada
            arquivos.push_back(caminho_completo);                                                             // adiciona à lista de resultados
        }
    
        for (const auto& filho : filhos) {                                                                    // aplica recursivamente a busca para todos os filhos
            filho.buscaPorExtensao(ext, arquivos);
        }
    }
    

    void encontraPastasVazias(vector<string>& vazias) const {
        if (tipo == "pasta") {                                      // se for pasta
            if (filhos.empty()) {
                vazias.push_back(caminho_completo);                 // se não tiver filhos, é vazia
            } else {
                for (const auto& filho : filhos) {                  // senão, verifica recursivamente os filhos
                    filho.encontraPastasVazias(vazias);
                }
            }
        }
    }
};

// Função que exporta a árvore de arquivos para um arquivo HTML
// @param raiz: Nó raiz da árvore de arquivos a ser exportada
// @param caminho: Caminho do arquivo HTML de saída
// @note Esta função cria um documento HTML completo com:
//       - Codificação UTF-8 para suporte a caracteres especiais
//       - Estilos CSS para formatação visual (cores e fontes)
//       - Estrutura hierárquica mantida através de indentação
// @example O arquivo HTML gerado terá:
//          - Arquivos em preto
//          - Diretórios em verde
//          - Fonte monoespaçada para alinhamento
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
            << "        body { \n"
            << "            font-family: monospace;\n"
            << "            white-space: pre;\n"
            << "            margin: 20px;\n"
            << "        }\n"
            << "        .arquivo { color: black; }\n"
            << "        .pasta { color: green; }\n"
            << "    </style>\n"
            << "</head>\n<body>\n"
            << raiz.gerarHTML(0, true)
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
                
            case 3: {
                int sub_opcao;
                cout << "\nPesquisas:\n";
                cout << "1. Maior arquivo\n";
                cout << "2. Arquivos por extensao\n";
                cout << "3. Pastas vazias\n";
                cout << "Digite: ";
                cin >> sub_opcao;

                if (sub_opcao == 1) {
                    long max_tam = -1;
                    vector<string> caminhos;
                    raiz.encontraMaiorArquivo(max_tam, caminhos);
                    cout << "\nMaior(es) arquivo(s):\n";
                    for (const auto& c : caminhos) {
                        cout << c << " (" << max_tam << " bytes)\n";
                    }
                } else if (sub_opcao == 2) {
                    string ext;
                    cout << "Extensao: ";
                    cin >> ext;
                    vector<string> arquivos;
                    raiz.buscaPorExtensao(ext, arquivos);
                    cout << "\nArquivos com extensao " << ext << ":\n";
                    for (const auto& arq : arquivos) {
                        cout << arq << "\n";
                    }
                } else if (sub_opcao == 3) {
                    vector<string> vazias;
                    raiz.encontraPastasVazias(vazias);
                    cout << "\nPastas vazias:\n";
                    for (const auto& pasta : vazias) {
                        cout << pasta << "\n";
                    }
                } else {
                    cout << "Opcao invalida.\n";
                }
                break;
            }
                
            case 4:
                cout << "\nSaindo...\n";
                break;
                
            default:
                cout << "\nOpcao invalida!\n";
        }
    } while (opcao_usuario != 4);

    return 0;
}
