###README###

#Założenia

Opracować na systemie Linux zestaw programów typu producent - konsument realizujących następujący schemat synchronicznej komunikacji międzyprocesowej:
Proces 1:	czyta dane ze standardowego strumienia wejściowego i przekazuje je w niezmienionej formie do procesu 2 poprzez mechanizm komunikacyjny K1.
Proces 2: 	pobiera dane przesłane przez proces 1. Konwertuje dane otrzymane z procesu 1 do postaci heksadecymalnej i przekazuje do procesu 3 poprzez mechanizm komunikacyjny K2.
Proces 3:	pobiera dane wyprodukowane przez proces 2 i wypisuje je na standardowym strumieniu diagnostycznym. Jednostki danych powinny zostać wyprowadzone po 15 w pojedynczym wierszu i oddzielone spacjami.

Wszystkie trzy procesy powinny być powoływane automatycznie z jednego procesu inicjującego. Po powołaniu procesów potomnych proces inicjujący wstrzymuje pracę. Proces inicjujący wznawia pracę w momencie kończenia pracy programu (o czym niżej), jego zadaniem jest „posprzątać” po programie przed zakończeniem działania.

Ponadto należy zaimplementować mechanizm asynchronicznego przekazywania informacji pomiędzy operatorem a procesami oraz pomiędzy procesami. Wykorzystać do tego dostępny mechanizm sygnałów.
Operator może wysłać do dowolnego procesu sygnał zakończenia działania (S1), sygnał wstrzymania działania (S2) i sygnał wznowienia działania (S3). Sygnał S2 powoduje wstrzymanie synchronicznej wymiany danych pomiędzy procesami. Sygnał S3 powoduje wznowienie tej wymiany. Sygnał S1 powoduje zakończenie działania oraz zwolnienie wszelkich wykorzystywanych przez procesy zasobów (zasoby zwalnia proces macierzysty).

Każdy z sygnałów przekazywany jest przez operatora tylko do jednego, dowolnego procesu. O tym, do którego procesu wysłać sygnał, decyduje operator, a nie programista. Każdy z sygnałów operator może wysłać do innego procesu. Mimo, że operator kieruje sygnał do jednego procesu, to pożądane przez operatora działanie musi zostać zrealizowane przez wszystkie trzy procesy. W związku z tym, proces odbierający sygnał od operatora musi powiadomić o przyjętym żądaniu pozostałe dwa procesy. Powinien wobec tego przekazać do nich odpowiedni sygnał informując o tym jakiego działania wymaga operator. Procesy odbierające sygnał, powinny zachować się adekwatnie do otrzymanego sygnału. Wszystkie trzy procesy powinny zareagować zgodnie z żądaniem operatora.
Sygnały oznaczone w opisie zadania symbolami S1 ÷ S3 należy wybrać samodzielnie spośród dostępnych w systemie (np. SIGUSR1, SIGUSR2, SIGINT, SIGCONT).

Mechanizmy komunikacji:

K1 - Kolejka komunikatów
K2 - Pamięć współdzielona

Program ma umożliwiać uruchomienie:
w trybie interaktywnym – operator wprowadza dane z klawiatury,
w trybie odczytu danych z określonego pliku,
w trybie odczytu danych z pliku /dev/urandom.

Dodatkowo wysyłanie sygnałów do procesów powinno odbywać się z wykorzystaniem dodatkowego programu napisanego w języku C. Program ten powinien umożliwiać (przy pomocy menu użytkownika) wybór sygnału oraz procesu do którego ten sygnał ma zostać wysłany.

#Opis uruchomienia

Aby uruchomić program należy na systemie Linuź uruchomić dwa terminale w ym samym folderze co pliki projektu. W pierwszym terminalu należy użyć komendy ./run1.sh by skompilować i uruchomić program główny projekt_so.c. Następnie w drugim terminalu należy użyć komendy ./run2.s by skompilować i uruchomić program do obsługi sygnałów sygnalu.c.

Uwaga: Podana wyżej kolejność uruchamiania programów jest konieczna do ich poprawnego działania.

#Opis implementacji

