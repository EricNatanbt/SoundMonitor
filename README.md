# SoundMonitor: Sistema Embarcado para Monitoramento Sonoro Inteligente

 Autor: Eric Natan Batista Torres
 
 Área temática: automação e controle sonoro em ambientes de grande porte
 
 O presente projeto apresenta o SoundMonitor, um sistema embarcado voltado para a 
monitoração sonora em tempo real. Seu objetivo central é auxiliar operadores de som em 
ambientes como igrejas, teatros e eventos ao vivo, permitindo que tenham um retorno preciso 
sobre os níveis sonoros percebidos pelo público. Para isso, o sistema utiliza um microfone 
para captar o som ambiente, exibindo os níveis em uma matriz de LEDs e um display OLED. 
Além disso, os dados são transmitidos via Wi-Fi para o ThingSpeak, onde são armazenados e 
transformados em gráficos para análise histórica. Os testes demonstraram a eficácia do 
sistema na identificação e sinalização de diferentes níveis de volume, proporcionando uma 
solução acessível e prática para o controle sonoro em grandes espaços.
 Palavras-chave: Monitoramento sonoro. IoT. Sistemas embarcados. Acústica. Automação.
 
 INTRODUÇÃO
 
Em ambientes como igrejas, teatros e eventos ao vivo, a qualidade do som 
desempenha um papel fundamental na experiência do público. No entanto, um dos principais 
desafios enfrentados pelos operadores de som é a dificuldade de avaliar com precisão o 
volume percebido pelos ouvintes, uma vez que a mesa de som geralmente está localizada em 
uma posição distante da plateia. Essa limitação pode resultar em ajustes inadequados de 
volume, comprometendo a inteligibilidade do som e o conforto dos ouvintes.
 Para solucionar esse problema, este projeto propõe o SoundMonitor, um sistema embarcado 
capaz de captar o som ambiente e fornecer um retorno visual ao operador de áudio. O sistema 
utiliza um microfone para medir os níveis sonoros, representando essas informações por meio 
de uma matriz de LEDs e um display OLED. Além disso, os dados são transmitidos via Wi
Fi para a plataforma ThingSpeak, permitindo a geração de gráficos que possibilitam uma 
análise histórica do comportamento sonoro no ambiente.
O objetivo principal do projeto é proporcionar aos operadores de som uma ferramenta 
confiável para monitoramento em tempo real, permitindo ajustes mais precisos e melhorando 
a experiência auditiva do público. Além disso, o sistema pode ser aplicado em diferentes 
contextos onde o responsável pelo controle do som está distante da fonte sonora, como em 
shows, conferências e auditórios.

METODOLOGIA

O desenvolvimento do SoundMonitor envolveu a utilização da placa BitDogLab, 
que integra os principais componentes necessários para o processamento do sinal sonoro e 
comunicação com a nuvem. Os principais elementos empregados no projeto incluem:
 - Botão A: Liga o sistema ao ser pressionado.
 - Botão B: Desliga o sistema ao ser segurado.
 - Microfone: Responsável pela captação do som ambiente e envio do sinal para 
conversão e análise.
 - Conversor Analógico-Digital (ADC): Utilizado para transformar o sinal analógico 
do microfone em valores digitais.
 - Matriz de LEDs: Empregada para fornecer um retorno visual rápido sobre o nível 
sonoro, alterando as cores conforme a intensidade do som, sendo azul para volumes 
baixos e muito baixos, verde para moderado e vermelho para alto e muito alto
 - Display OLED: Exibe mensagens de inicialização e desligamento e informações 
detalhadas, como o nível de volume e o valor medido em decibéis (dB).
 - Conectividade Wi-Fi: Permite a transmissão dos dados para o ThingSpeak via 
protocolo HTTP.

*DESENVOLVIMENTO*

 O desenvolvimento deste projeto foi estruturado em diversas etapas, abrangendo desde a 
criação de diagramas e a captação do som até a transmissão segura dos dados para a nuvem. 
Cada fase foi planejada com rigor para assegurar precisão, eficiência e confiabilidade no 
monitoramento sonoro.

1. Captação e Processamento do Som
 A captação do som ambiente é realizada por um microfone, que converte as ondas sonoras 
em sinais elétricos. Esses sinais são então transmitidos para um conversor ADC (Analog-to
Digital Converter), responsável por transformar os valores analógicos em dados digitais, 
permitindo a manipulação pelo microcontrolador.
 Para otimizar o processamento dos dados e reduzir a carga sobre a CPU, o sistema utiliza 
DMA (Direct Memory Access), permitindo que as transferências de dados do ADC para a 
memória ocorram sem a intervenção direta do processador. Isso garante uma amostragem 
precisa e contínua dos níveis sonoros, reduzindo latências e minimizando ruídos indesejados. 
Dessa forma, o microcontrolador pode realizar outras tarefas simultaneamente, aumentando a 
eficiência e a confiabilidade do sistema.

 2. Classificação dos Níveis Sonoros
 O microcontrolador recebe os valores digitalizados e processa-os por meio de um algoritmo 
de categorização de som. Esse algoritmo compara os valores captados com um conjunto de 
faixas predefinidas (definidas de acordo com os testes realizados no microfone):
 - Baixo: Até 43 dB
 - Moderado: Entre 44 dB e 55 dB
 - Alto: Entre 56 dB e 60 dB
 - Muito Alto: Acima de 60 dB
 Essa classificação é essencial para determinar as reações do sistema e garantir que os alertas 
visuais e a transmissão de dados ocorram de forma correta.

 3. Exibição das Informações
 Com base na classificação do nível sonoro, o sistema utiliza diferentes meios para exibir as 
informações:
 - Matriz de LEDs: Altera sua cor conforme o nível sonoro detectado:
 - Azul: Som baixo
 - Verde: Som moderado
 - Vermelho: Som alto
- Vermelho: Som muito alto
  - Display OLED: As informações são exibidas em um display OLED, que utiliza o 
protocolo I2C para comunicação, permitindo uma integração eficiente com o sistema. 
Esse display apresenta a leitura exata em decibéis, proporcionando ao usuário uma 
visualização clara e imediata dos níveis sonoros captados. 
Essa abordagem combinada de feedback visual e numérico assegura maior precisão na 
interpretação dos níveis sonoros.

 4. Transmissão para a Nuvem
 Os dados coletados são enviados para a plataforma ThingSpeak a cada 15 segundos, 
respeitando a limitação imposta pela versão gratuita do serviço. A transmissão ocorre via Wi
Fi, utilizando o protocolo HTTP para garantir a comunicação eficiente com a nuvem.
 Os valores armazenados na plataforma são utilizados para:
 - Gerar gráficos históricos dos níveis sonoros.
 - Simular remotamente o comportamento dos LEDs, proporcionando um 
acompanhamento visual em tempo real.
 - Permitir a análise de padrões e tendências sonoras ao longo do tempo.
 A integração com o ThingSpeak amplia a utilidade do sistema, possibilitando o acesso e a 
análise dos dados de qualquer local, facilitando o monitoramento remoto.

 CONCLUSÃO
 
 O projeto SoundMonitor atingiu seus objetivos ao oferecer um sistema de 
monitoramento sonoro acessível e funcional para operadores de som em ambientes amplos. O 
sistema demonstrou ser eficiente na captação e exibição dos níveis sonoros, contribuindo para 
um melhor ajuste do volume e aprimoramento da experiência auditiva do público.
 Os principais desafios enfrentados envolveram as limitações do microfone utilizado, que 
impactaram a precisão da captação sonora, além das restrições do banco de dados da nuvem 
utilizado, o ThingSpeak na versão gratuita, que limitou a quantidade de dados armazenados e 
a frequência de atualizações. No entanto, o sistema se mostrou promissor, validando sua 
proposta inicial.
 Como melhorias futuras, pretende-se substituir o microfone por um modelo de maior 
qualidade, aprimorar os algoritmos de processamento de áudio e explorar a integração com 
outros dispositivos para expandir suas funcionalidades. Também está prevista a migração 
para uma versão paga do ThingSpeak ou a utilização de outras soluções de banco de dados 
em nuvem, visando melhorar o armazenamento e a escalabilidade.
 Em conclusão, o SoundMonitor representa uma solução inovadora e viável para a 
monitoração sonora em tempo real, sendo uma ferramenta útil para profissionais da área de 
áudio e automação.

LINKS DO PROJETO

Vídeo de apresentação:
https://youtu.be/ffg8oL80Uyo

ThingSpeak: 
https://thingspeak.mathworks.com/channels/2836790
