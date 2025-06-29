#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <unordered_set>
#include <utility>

using namespace std;
namespace fs = filesystem;

/*
----------Compilação----------
g++ -std=c++17 main.cpp -o main
./main [diretorio_opcional]
*/

class FileNode
{
public:
    string nome;
    string tipo;  // "arquivo" ou "pasta"
    long tamanho; // em bytes (0 para pastas)
    vector<FileNode> filhos;
    string caminho_completo;

    // Construtor: Inicializa um nó da árvore com nome, tipo ("arquivo" ou "pasta") e tamanho
    // @param nome: Nome do arquivo ou pasta
    // @param tipo: Tipo do nó ("arquivo" ou "pasta")
    // @param tamanho: Tamanho em bytes (0 por padrão, usado principalmente para arquivos)
    FileNode(const string &nome, const string &tipo, long tamanho = 0, const string &caminho = "")
        : nome(nome), tipo(tipo), tamanho(tamanho), caminho_completo(caminho) {}

    // Adiciona um nó filho (arquivo ou pasta) ao nó atual
    // @param filho: Nó a ser adicionado como filho
    void adicionarFilho(const FileNode &filho)
    {
        filhos.push_back(filho);
    }

    // Retorna o número de filhos diretos (não recursivo) do nó atual
    // @return O número total de nós filhos imediatos (arquivos e pastas)
    // @note Este método conta apenas os filhos diretos, não incluindo subpastas
    //       ou arquivos dentro das subpastas. Para uma contagem completa de
    //       todos os itens na árvore, seria necessário um método recursivo.
    // @see calcularTamanhoTotal() para um exemplo de método recursivo
    int contarFilhos() const
    {
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
    long calcularTamanhoTotal() const
    {
        if (tipo == "arquivo")
        {
            return tamanho;
        }

        long tamanho_total = 0;
        for (const auto &filho : filhos)
        {
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
    void mostrar(int nivel = 0, bool ultimo = true) const
    {
        // Indentação inicial
        if (nivel > 0)
        {
            for (int i = 0; i < nivel - 1; i++)
            {
                cout << "|   ";
            }
            cout << (ultimo ? "└── " : "├── ");
        }

        // Nome e informações
        cout << nome;
        if (tipo == "arquivo")
        {
            cout << " (" << tamanho << " bytes)";
        }
        else
        {
            int num_filhos = contarFilhos();
            long tamanho_total = calcularTamanhoTotal();
            cout << " (" << num_filhos << (num_filhos == 1 ? " filho" : " filhos")
                 << ", " << tamanho_total << " bytes)";
        }
        cout << endl;

        // Mostra filhos com a linha vertical
        if (!filhos.empty())
        {
            for (size_t i = 0; i < filhos.size(); i++)
            {
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
    string gerarHTML(int nivel = 0, bool ultimo = true) const
    {

        string html;

        // Indentação inicial
        for (int i = 0; i < nivel; i++)
        {
            html += (i == nivel - 1 ? (ultimo ? "└── " : "├── ") : "│   ");
        }

        // Nome e informações
        html += "<span class='" + tipo + "'>" + nome;
        if (tipo == "arquivo")
        {
            html += " (" + to_string(tamanho) + " bytes)";
        }
        else
        {
            int num_filhos = contarFilhos();
            long tamanho_total = calcularTamanhoTotal();
            html += " (" + to_string(num_filhos) + (num_filhos == 1 ? " filho" : " filhos") + ", " + to_string(tamanho_total) + " bytes)";
        }
        html += "</span><br>\n";

        // Adiciona filhos
        if (!filhos.empty())
        {
            for (size_t i = 0; i < filhos.size(); i++)
            {
                html += filhos[i].gerarHTML(nivel + 1, i == filhos.size() - 1);
            }
        }

        return html;
    }
    // função recursiva que encontra o(s) arquivo(s) de maior tamanho na árvore de arquivos
    // max_tam: referência para o maior tamanho encontrado até o momento
    // caminhos: referência para um vetor que acumula os caminhos dos arquivos de maior tamanho
    // a função atualiza max_tam e caminhos conforme percorre a árvore
    void encontraMaiorArquivo(long &max_tam, vector<string> &caminhos) const
    {
        if (tipo == "arquivo")
        { // se o nó for arquivo
            if (tamanho > max_tam)
            {                                         // e se for maior que o tamanho atual máximo
                max_tam = tamanho;                    // atualiza o maior tamanho
                caminhos.clear();                     // limpa os caminhos anteriores
                caminhos.push_back(caminho_completo); // adiciona o novo maior
            }
            else if (tamanho == max_tam)
            {
                caminhos.push_back(caminho_completo); // se for do mesmo tamanho do maior, adiciona também
            }
        }
        for (const auto &filho : filhos)
        { // aplica recursivamente para todos os filhos (arquivos e subpastas)
            filho.encontraMaiorArquivo(max_tam, caminhos);
        }
    }
    // função recursiva que busca todos os arquivos com uma extensão específica dentro da árvore.
    // ext: extensão procurada (ex: ".txt", ".cpp").
    // arquivos: vetor que armazena os caminhos completos dos arquivos encontrados
    // visitados: conjunto de caminhos já visitados (para evitar loops com links simbólicos ou duplicações)
    // utiliza std::filesystem para extrair a extensão de forma robusta
    //       Evita a repetição de diretórios já visitados usando uma hash set.
    //       Busca em profundidade (DFS) por toda a árvore.
    void buscaPorExtensao(const string &ext, vector<string> &arquivos, std::unordered_set<string> &visitados) const
    {
        if (visitados.count(caminho_completo)) return; // Prevenir loops infinitos, ele verifica se o caminho já foi visitado
        visitados.insert(caminho_completo);
        // não funciona para arquivos com ponto no nome, pois busca pela extensão literalmente
        if (tipo == "arquivo" && fs::path(nome).extension() == ext)
        {
            arquivos.push_back(caminho_completo); // adiciona à lista de resultados
        }

        for (const auto &filho : filhos)
        { // aplica recursivamente a busca para todos os filhos
            filho.buscaPorExtensao(ext, arquivos, visitados);
        }
    }

    // função recursiva que encontra todas as pastas vazias na árvore de arquivos
    // vazias: vetor que armazena os caminhos completos das pastas sem nenhum filho
    // uma pasta é considerada vazia se não possui nem arquivos nem subpastas diretamente dentro dela
    void encontraPastasVazias(vector<string> &vazias) const
    {
        if (tipo == "pasta")
        { // se for pasta
            if (filhos.empty())
            {
                vazias.push_back(caminho_completo); // se não tiver filhos, é vazia
            }
            else
            {
                for (const auto &filho : filhos)
                { // senão, verifica recursivamente os filhos
                    filho.encontraPastasVazias(vazias);
                }
            }
        }
    }
    // Função recursiva que encontra todos os arquivos cujo tamanho é maior que N bytes.
    // Os arquivos encontrados são adicionados ao vetor 'arquivos' como pares <caminho, tamanho>.
    void buscaArquivosMaiores(long n, vector<pair<string, long>> &arquivos) const
    {
        // Verifica se o nó é um arquivo e se seu tamanho é maior que n
        if (tipo == "arquivo" && tamanho > n)
        {
            // Se a condição for satisfeita, adiciona o caminho completo e o tamanho ao vetor de resultados.
            arquivos.push_back({caminho_completo, tamanho});
        }

        // Itera por todos os filhos do nó atual (caso seja uma pasta) e aplica a função recursivamente.
        for (const auto &filho : filhos)
        {
            filho.buscaArquivosMaiores(n, arquivos);
        }
    }
    // Função recursiva que encontra a pasta com o maior número de arquivos diretamente dentro dela (não recursivo).
    void encontraPastaComMaisArquivos(int &max_arquivos, string &caminho_pasta) const
    {
        // Verifica se o nó atual é uma pasta.
        if (tipo == "pasta")
        {
            int arquivos_diretos = 0; // Contador para arquivos diretamente nesta pasta.
            // Itera sobre os filhos imediatos do nó atual.
            for (const auto &filho : filhos)
            {
                // Se o filho for um arquivo, incrementa o contador.
                if (filho.tipo == "arquivo")
                {
                    arquivos_diretos++;
                }
            }
            // Se a quantidade de arquivos diretos nesta pasta for maior do que a máxima encontrada até agora, atualiza os valores de referência.
            if (arquivos_diretos > max_arquivos)
            {
                max_arquivos = arquivos_diretos;
                caminho_pasta = caminho_completo;
            }
            // Chama recursivamente a função para os filhos (subpastas) do nó atual.
            for (const auto &filho : filhos)
            {
                filho.encontraPastaComMaisArquivos(max_arquivos, caminho_pasta);
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
void exportarHTML(const FileNode &raiz, const string &arquivo_saida)
{
    ofstream arquivo(arquivo_saida);
    if (!arquivo.is_open())
    {
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
    cout << "Arquivo HTML gerado com sucesso: " << arquivo_saida << endl;
}

// Função que carrega a árvore de diretórios REAL
FileNode carregarArvore(const fs::path &caminho, std::unordered_set<std::string> &visitados)
{
    // Evita visitar o mesmo caminho duas vezes (ex: links simbólicos)
    std::string caminhoStr = fs::canonical(caminho).string();
    if (visitados.count(caminhoStr)) return FileNode(caminho.filename().string(), "pasta", 0, caminhoStr);
    visitados.insert(caminhoStr);

    FileNode raiz(caminho.filename().string(), "pasta", 0, caminhoStr);

    try
    {
        for (const auto &entry : fs::directory_iterator(caminho, fs::directory_options::skip_permission_denied))
        {
            try
            {
                if (!entry.is_symlink() && !entry.is_block_file() && !entry.is_character_file())
                {
                    if (entry.is_directory())
                    {
                        FileNode subpasta = carregarArvore(entry.path(), visitados);
                        raiz.adicionarFilho(subpasta);
                    }
                    else if (entry.is_regular_file())
                    {
                        raiz.adicionarFilho(FileNode(
                            entry.path().filename().string(),
                            "arquivo",
                            entry.file_size(),
                            entry.path().string()));
                    }
                }
            }
            catch (const fs::filesystem_error &e)
            {
                std::cerr << "Aviso interno: não foi possível acessar \"" << entry.path()
                          << "\": " << e.what() << std::endl;
            }
        }
    }
    catch (const fs::filesystem_error &e)
    {
        std::cerr << "Aviso: não foi possível acessar \"" << caminho << "\": " << e.what() << std::endl;
    }

    return raiz;
}

int main(int argc, char *argv[])
{
    std::unordered_set<std::string> visitados;
    fs::path diretorio_base = (argc > 1) ? argv[1] : fs::current_path();
    cout << "Diretorio inicial: " << diretorio_base << "\n\n";

    // Carrega a árvore REAL do sistema de arquivos
    FileNode raiz = carregarArvore(diretorio_base, visitados);

    // Menu principal
    int opcao_usuario;
    do
    {
        cout << "\nOpcoes:\n";
        cout << "1. Exibir arvore\n";
        cout << "2. Exportar HTML\n";
        cout << "3. Pesquisar\n";
        cout << "4. Sair\n";
        cout << "Digite: ";
        cin >> opcao_usuario;

        switch (opcao_usuario)
        {
        case 1:
            cout << "\nEstrutura:\n";
            raiz.mostrar();
            break;

        case 2:
        {
            string arquivo_saida = "arvore.html";
            cout << "\nExportando para HTML...\n";
            exportarHTML(raiz, arquivo_saida);
            break;
        }

        case 3:
        {
            int sub_opcao;
            cout << "\nPesquisas:\n";
            cout << "1. Maior arquivo\n";
            cout << "2. Arquivos por extensao\n";
            cout << "3. Pastas vazias\n";
            cout << "4. Arquivos maiores que N bytes\n";
            cout << "5. Pasta com mais arquivos diretos\n";
            cout << "Digite: ";
            cin >> sub_opcao;

            if (sub_opcao == 1)
            {
                long max_tam = -1;                            // inicializa max_tam com -1 para representar um tamanho inicial inválido
                vector<string> caminhos;                      // vetor caminhos para armazenar o caminho do maior arquivo
                raiz.encontraMaiorArquivo(max_tam, caminhos); // chama a função recursiva a partir da raiz da árvore
                if (caminhos.empty())
                {
                    cout << "\nSem arquivos para esta seleção\n";
                }
                else
                {
                    cout << "\nMaior(es) arquivo(s):\n";
                    for (const auto &c : caminhos)             // caso haja resultados, imprime o caminho de cada arquivo encontrado junto com seu tamanho
                    {
                        cout << c << " (" << max_tam << " bytes)\n";
                    }
                }
            }
            else if (sub_opcao == 2)    
            {
                std::unordered_set<std::string> visitados;       // unordered_set para registrar os caminhos já visitados para evitar loops infinitos
                string ext;
                cout << "Extensao: ";                            // solicita ao usuário a extensão a ser buscada
                cin >> ext;
                vector<string> arquivos;                         // chama a função recursiva para buscar arquivos com a extensão especificada, armazenando os caminhos no vetor arquivos
                raiz.buscaPorExtensao(ext, arquivos, visitados); 
                
                if (arquivos.empty())
                {
                    cout << "\nSem arquivos para esta seleção\n"; // exibe uma mensagem caso nenhum arquivo tenha sido encontrado
                }
                else                                              // se houver arquivos, imprime seus caminhos completos no console
                {
                    cout << "\nArquivos com extensao " << ext << ":\n";
                    for (const auto &arq : arquivos)
                    {
                        cout << arq << "\n";
                    }
                }
            }
            else if (sub_opcao == 3)
            {
                vector<string> vazias;                // vetor "vazias" para armazenar os caminhos das pastas que não possuem nenhum filho (nem arquivos, nem subpastas).
                raiz.encontraPastasVazias(vazias);    // chama a função de forma recursiva 
                cout << "\nPastas vazias:\n";         // imprime a lista de pastas vazias encontradas
                for (const auto &pasta : vazias)
                {
                    cout << pasta << "\n";
                }
            }
            else if (sub_opcao == 4)
            {
                long n;                                                 // solicita ao usuário o valor N em bytes para comparar com os tamanhos dos arquivos
                cout << "Digite o valor N (em bytes): ";                
                cin >> n;
                vector<pair<string, long>> arquivos;                    //  executa a busca recursiva, armazenando os arquivos maiores que N como pares <caminho, tamanho>.
                raiz.buscaArquivosMaiores(n, arquivos);
                cout << "\nArquivos maiores que " << n << " bytes:\n"; 
                for (const auto &[caminho, tam] : arquivos)
                {
                    cout << caminho << " (" << tam << " bytes)\n";      // lista os arquivos encontrados e seus respectivos tamanhos
                }
            }
            else if (sub_opcao == 5)
            {
                int max_arquivos = -1;                                                  // inicia a variável max_arquivos com -1 e caminho_pasta vazio
                string caminho_pasta;
                raiz.encontraPastaComMaisArquivos(max_arquivos, caminho_pasta);         // chama função recursiva que atualiza "max_arquivos" com a maior quantidade de arquivos diretos e armazena o caminho da pasta correspondente
                if (max_arquivos >= 0)                                                  // se encontrou alguma pasta com arquivos diretos, exibe a pasta e o número de arquivos
                {
                    cout << "\nPasta com mais arquivos diretos:\n";
                    cout << caminho_pasta << " (" << max_arquivos << " arquivo(s))\n";
                }
                else
                {
                    cout << "\nNenhuma pasta encontrada.\n";                             // se não encontrou nenhuma pasta com arquivos, exibe mensagem alternativa.
                }
            }
            else
            {
                cout << "Opcao invalida.\n";     // mensagem caso a sub_opcao seja invalida
            }
            break;
        }

        case 4:
            cout << "\nSaindo...\n";
            break;

        default:                                 // default captura qualquer entrada inválida do menu principal
            cout << "\nOpcao invalida!\n";   
        }
    } while (opcao_usuario != 4);                // o programa continua executando enquanto o usuário não escolher sair

    return 0;
}
