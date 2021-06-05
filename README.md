##
# **TIN – dokumentacja wstępna**

#
### Skład zespołu

- Kacper Kostecki (kkosteck@mion.elka.pw.edu.pl)
- Katarzyna Kulpa
- Piotr Szachewicz
- Mikołaj Gutowski

#
### Treść zadania

Napisać program obsługujący prosty protokół P2P (Peer-to-Peer).

## Założenia

- Zasób to plik identyfikowany pewną nazwą. Zasoby o takich samych nazwach uważa się za identyczne.
- Początkowo dany zasób znajduje się w jednym węźle sieci, następnie może być propagowany do innych węzłów w ramach transferu inicjowanego ręcznie przez klienta (patrz dalej) – raz pobrany zasób zostaje zachowany jako kopia.
- Tak więc, po pewnym czasie działania systemu ten sam zasób może znajdować się w kilku węzłach sieci (na kilku maszynach).
- Program ma informować o posiadanych lokalnie (tj. w danym węźle) zasobach i umożliwiać ich pobranie.
- Program powinien umożliwiać współbieżne:
  - wprowadzanie przez klienta (poprzez interfejs tekstowy):
    - nowych zasobów – z lokalnego systemu plików,
    - poleceń pobrania nazwanego zasobu ze zdalnego węzła,
  - pobieranie zasobów „w tle&quot; (także kilku jednocześnie),
  - rozgłaszanie informacji o posiadanych lokalnie zasobach.
- W przypadku pobierania zdalnego zasobu system sam (nie klient) decyduje skąd zostanie on pobrany.
- Zasób pobrany do lokalnego węzła jest kopią oryginału, kopia jest traktowana tak samo jak oryginał (są nierozróżnialne) – tj.: istnienie kopii jest rozgłaszane tak samo jak oryginału. {Duplikat}
- Właściciel zasobu może go unieważnić wysyłając odpowiedni komunikat rozgłaszany. Wszystkie kopie zasobu po unieważnieniu powinny przestać być rozgłaszane. W przypadku gdy trwają transfery zasobu, który został unieważniony powinny się one poprawnie skończyć, dopiero wtedy informacja o zasobie powinna być usunięta. {Kasowanie}
- Należy zwrócić uwagę na obsługę różnych sytuacji wyjątkowych – np. przerwanie transmisji spowodowane błędem sieciowym.
- Lokalizacja zasobów ma następować poprzez rozgłaszanie – wskazówka: użyć prot. UDP, ustawić opcje gniazda SO\_BROADCAST, wykorzystać adresy IP rozgłaszające (same bity &quot;1&quot; w części hosta).
- Można dodatkowo wprowadzić skrót lub podpis, aby zapewnić jednoznaczność identyfikacji zasobów.
- Interfejs klienta – wystarczy prosty interfejs tekstowy

## Wariant

W2 - Powinno być możliwe pobranie zasobu z kilku węzłów na raz (tj. &quot;w kawałkach&quot;).

#
### Doprecyzowanie treści zadania

- Lokalne zasoby przechowywane są w specjalnym folderze – zakładamy, że pliki w nim nie mogą być modyfikowane
- Dodanie plików do lokalnego zasobu polega na skopiowaniu ich z lokalnego systemu plików do specjalnego folderu z przechowywanymi zasobami lokalnymi
- Zasoby będą identyfikowane za pomocą unikalnego ID
- Jeżeli dany klient pobierze częściowo jakiś zasób to może już go zacząć udostępniać
- Każdy klient, który w pełni pobierze zasób, będzie miał prawa do jego skasowania
- Długość nazwy zasobów będzie ograniczona do 48 znaków.

#
### Krótki opis funkcjonalny

- Lista zasobów:
  - Każdy klient co pewien czas będzie wysyłał datagram z listą posiadanych lokalnie zasobów na adres rozgłoszeniowy
  - Wraz z nazwami plików przesyłane są również ich rozmiar, które są potrzebne, aby wyznaczyć punkty podziału danego pliku na kawałki.
  - Oprócz listy w pełni pobranych zasobów przesyłane są również informacje o częściowo pobranych zasobach. Dzięki temu nie jest konieczne łączenie się z każdym klientem, żeby ustalić, czy posiada dane kawałki zasobu
- Pobieranie zasobu:
  - Każdy zasób w momencie transferu przez pierwszego klienta zostaje podzielony na kawałki o równych rozmiarach:
    - Plik nie będzie fizycznie dzielony na dysku na poszczególne pliki – kawałki będą otrzymywane poprzez bezpośrednie sięganie do pliku
    - Poszczególny kawałek posiada offset mówiący o pozycji, w której kawałek zaczyna się w danym pliku
    - Każdy kawałek ma stały rozmiar wstępnie ustalony jako 8 KB, ale jeżeli zajdzie potrzeba rozmiar będzie zmieniany
  - W momencie chęci pobrania danego zasobu klient losowo wybiera innego klienta, od którego pobierze dany kawałek i próbuje się z nim połączyć
  - Jeżeli nie uda się nawiązać połączenia to poszukiwany jest kolejny host
  - Operacje powtarzane są dla każdego kawałka zasobu
  - Połączenie pozostaje otwarte, dopóki klient w pełni nie pobierze danego zasobu
  - Jeżeli zasób zostanie w pełni pobrany połączenie zostaje zamknięte
- Usuwanie zasobu:
  - Jeżeli zasób zostanie unieważniony przez danego klienta to wysłany zostaje datagram z informacją o unieważnieniu na adres rozgłoszeniowy
  - Informacja o unieważnieniu jest wysyłana również w momencie, kiedy któryś z klientów otrzyma informacje o ważności zasobu, który został wcześniej unieważniony

#
### Opis i analiza poprawności stosowanych protokołów komunikacyjnych

- UDP: rozgłaszanie list zasobów oraz próśb o usunięcie zasobu
  - Przesyłanie listy zasobów każdemu klientowi z osobna poprzez połączenia TCP byłoby zbyt czasochłonne
  - Istnieje możliwość zgubienia pakietu, ale nie jest to bardzo znaczące, ponieważ wszystkie pakiety przesyłane przy użyciu UDP są rozgłaszane poprzez adres rozgłoszeniowy co stały okres czasu, więc ten sam pakiet zostanie wysłany ponownie za pewien interwał czasu
- TCP: transfer zasobów
  - Informacja zwrotna dotycząca dostarczenia pakietu
  - W przypadku zerwania połączenia, pobierający może poszukać innego udostępniającego

#
### Planowany podział na moduły i struktura komunikacji między nimi

- Moduł główny
  - Zarządza pozostałymi modułami
- Moduł wysyłania
  - Nasłuchuje na porcie TCP
  - Każde połączenie jest obsługiwane w oddzielnym wątku
- Moduł rozgłoszeniowy
  - Wysyła informacje o liście zasobów oraz usunięciu pliku na adres rozgłoszeniowy
- Moduł pobierania
  - Klient TCP
  - Wybieranie węzła, z którego pobierze zasób
  - Obsługiwanie listy dostępnych zasobów
- Interfejs
  - Dodawanie i unieważnianie zasobów
  - Lista dostępnych zasobów
  - Lista pobieranych zasobów

#
### Definicje ramek

## Datagramy UDP
```
struct DatagramHeader {
  uin32_t type;
}; \\ 4B
```
Rodzaj pakietu będzie rozpoznawany poprzez z góry ustalony typ (np. pakiet z listą zasobów: type = 1, pakiet z informacją o usuniętym zasobie type = 2), który będzie przesyłany w nagłówku.
```
struct ListDatagramHeader { 
DatagramHeader header; 
  uint32_t size; 
}; \\ 8B 
```
Specjalny nagłówek dla pakietów, w których wysyłana jest lista zasobów. Zawiera główny nagłówek oraz rozmiar listy.
```
struct DeleteDatagramHeader { 
  DatagramHeader header; 
  char resource_id[16]; 
}; \\ 20B 
```
Nagłówek z informacją o unieważnieniu zasobu. Zawiera główny nagłówek oraz id zasobu.
```
struct ResourceDetails { 
  char resource_id[16]; 
  uint32_t size; 
  char name[48]; 
}; \\ 68B 
```
Pojedyncza struktura opisująca jedną pozycję na liście zasobów. Przesyłana jest wraz z odpowiednim nagłówkiem. Aby ograniczyć rozmiar przesyłanego pakietu, dopuszczalne będzie przesłanie 8 pozycji, co da rozmiar równy 552B.

## Pakiety TCP
```
struct PacketHeader { 
  uint8_t command; 
};  
```
Nagłówek zawierający polecenie do wykonania np. SendFile, EndOfFile itp.
```
struct ResourcePacketHeader {  
  uint8_t command; 
  uint32_t offset; 
  uint64_t size; 
}; 
```
Specjalny nagłówek użyty w przypadku wysyłania danych zasobu. Posiada pozycję początkową przesyłanego kawałka pliku oraz jego rozmiar. Wraz z tym nagłówkiem przesyłany jest jeden kawałek pliku.

#
### Zakres koncepcji implementacji

- Język:
  - C++
- Środowisko:
  - Linux
- Biblioteki: standardowe biblioteki C:
  - sys/socket.h
  - netinet/in.h
  - itd.
- Narzędzia:
  - CMake
