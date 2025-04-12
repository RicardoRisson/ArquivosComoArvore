#include <iostream>
#include <filesystem>
#include <string>

using namespace std;
namespace fs = filesystem;
/*
informaçoes importantes pra nao se perder
----------Compilação----------
pode rodar por linha de codigo ou so com o compilador mesmo
so pra criar o arquivo q ta funcionando assim p mim a

----------Como rodar----------
./main (roda o diretorio atual)
./main /diretorio/desejado
*/

int main(int argc, char* argv[]) {
    // 1. Define o diretório base
    fs::path diretorio_base = (argc > 1) ? argv[1] : fs::current_path();

    cout << "Sistema de Arquivos - Visualizacao em Arvore\n";
    cout << "Diretorio inicial: " << diretorio_base << "\n\n";

    // 2. Menu principal
    int opcao_usuario;
    do {
        cout << "Opcoes disponiveis:\n";
        cout << "1. exibir arvore de arquivos\n";
        cout << "2. exportar para HTML\n";
        cout << "3. opcoes de pesquisa\n";
        cout << "4. sair\n";
        cout << "digite sua opcao: ";
        cin >> opcao_usuario;

        switch (opcao_usuario) {
            case 1:
                cout << "\nestrutura do diretorio:\n";
                cout << "vamo exibir a arvore aqui\n";
                break;
                
            case 2:
                cout << "\naqui vamo exportar pra html\n";
                break;
                
            case 3:
                cout << "\nmenu de pesquisa:\n";
                cout << "aqui vai ter as opçoes\n";
                break;
                
            case 4:
                cout << "\nencerra o programa\n";
                break;
                
            default:
                cout << "\nOpcao invalida! Tente novamente.\n";
        }
        
    } while (opcao_usuario != 4);

    return 0;
}