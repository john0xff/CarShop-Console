/*----------------------------------------------------------------------------

 autokomis.c

 Program demonstruje metody wykorzystania zmiennych strukturalnych i tablic
 struktur jako narzêdzia realizacji ewidencji pojazdow dla autokomisu. To
 jest program o charakterze edukacyjnym, zatem zastosowano uproszczenia 
 typowe dla tej klasy programow. Program mocno "pracuje" na zmiennych 
 globalnych:
    - pojazdy,
    - lb_pojazdow.
 Zastosowano prosty mechanizm przegl¹dania zawartoœci ewidencji (sekwencyjnie,
 kolejne pojazdy), usuwanie w trakcie przegl¹dania.
 
 Program stanowi uzupelnienie wykladu z podstaw programowania w jezykach 
 C/C++,  poswieconych typom strukturalnym oraz operacjom na plikach struktur.

 Autor: Roman Siminski

 Wersja dedykowana dla kompilatorow C89
 
 Uwaga. Program wykonuje czyszczenie bufora strumienia stdin z wykorzystaniem 
 funkcji fflush. Z powodu róznych implementacji tej funkcji, czyszczenie moze 
 nie dzialac w systemach Linuxowych.
 
----------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Maksymalna dlugosc pol marka i model */
#define MAKS_M 20

/* Maksymalna dlugosc pola numeru rejestracyjnego */
#define MAKS_R 10

/* Maksymalna liczba ewidencjonowanych pojazdow  */
#define MAKS_P 200

/* Deklaracja typu strukturalnego, opisu informacji o pojezdzie */
struct _pojazd
{
  char  marka[ MAKS_M ];
  char  model[ MAKS_M ];
  short int rok_prod;
  float cena;
  float przebieg;
  char  nr_rej[ MAKS_R ];
};

typedef struct _pojazd pojazd;

/* Tablica struktur opisujacych pojazdy */
pojazd pojazdy[ MAKS_P ];

/* Nazwa pliku danych ewidencji pojazdow */
const char nazwa_pliku[] = "pojazdy.dat";

/* Aktualna liczba ewidencjonowanych pojazdow */
short int lb_pojazdow;

/* Prototypy funkcji */
void inicjalizacja( void );
void zakonczenie( void );
void menu( void );
void dopisz_pojazd( void );
void pokaz_pojazdy( void );
void szukaj_pojazdu( void );
void usun_pojazd( void );
void usun_wszystko( void );
void z_pliku_do_tablicy( void );
void z_tablicy_do_pliku( void );
void czytaj_info( pojazd * info );
void pokaz_info( const pojazd * info );
void usun_z_ewidencji( int nr );
void szukaj_wg_marki( void );
void szukaj_wg_przebiegu( void );
void przewin_ekran( void );

int main()
{
  int opcja;

  inicjalizacja(); /* Czytaj zawartsc pliku do tablicy pojazdy */
  do
  {
    menu();
    opcja = tolower( getchar() ); fflush( stdin );
    switch( opcja )
    {
       case 'd' : dopisz_pojazd();
                  break;
       case 'p' : pokaz_pojazdy();
                  break;
       case 's' : szukaj_pojazdu();
                  break;
       case 'u' : usun_pojazd();
                  break;
       case 'w' : usun_wszystko();
                  break;
    }
  }
  while( opcja != 'k' );

  zakonczenie(); /* Zapisz zawartsc tablicy pojazdy do pliku */

  return EXIT_SUCCESS;
}

/*----------------------------------------------------------------------------
  Inicjalizacja ewidencji. Zaladowanie danych z pliku do tablicy pojazdy. 
----------------------------------------------------------------------------*/
void inicjalizacja( void )
{
  przewin_ekran();
  puts( "Autokomis - ewidencja pojazdow. Wersja demo." )   ;
  z_pliku_do_tablicy();
  if( lb_pojazdow == 0 )
    printf( "\nEwidencja jest pusta." );
  else
    printf( "\nLiczba ewidencjonowanych pojazdow: %d.", lb_pojazdow );
 
  printf( "\nNacisnij Enter by rozpoczac program..." );
  ( void )getchar(); fflush( stdin );
}

/*----------------------------------------------------------------------------
  Zapisanie ewidencji z tablicy pojazdy do pliku.
----------------------------------------------------------------------------*/
void zakonczenie( void )
{
  przewin_ekran();
  z_tablicy_do_pliku();
  printf( "\nNacisnij Enter by zakonczyc program..." );
  ( void )getchar();
}

/*----------------------------------------------------------------------------
  Wyswietlenie glownego menu programu.
----------------------------------------------------------------------------*/
void menu( void )
{
  przewin_ekran();
  printf( "\n\nEwidencja pojazdow:" );
  printf( "\n D - dopisz pojazd,\n P - pokaz pojazdy,\n S - szukaj pojazdu,");
  printf( "\n U - usun pojazd,\n W - usun wszystkie,\n K - koniec.\n?>" );
}

/*----------------------------------------------------------------------------
  Dopisanie kolejnych pojazdow do ewidencji.
----------------------------------------------------------------------------*/
void dopisz_pojazd( void )
{
  int jeszcze_jeden;
  
  if( lb_pojazdow < MAKS_P ) /* Czy jest miejsce w tablicy? */
    do
    {
      przewin_ekran();
  
      czytaj_info( &pojazdy[ lb_pojazdow ] );
      lb_pojazdow++;

      printf( "\nCzy wprowadzasz nastepny pojazd? (t/n): " );

      jeszcze_jeden = ( tolower( getchar() ) == 't' );
      fflush( stdin );

    }
    while( jeszcze_jeden && lb_pojazdow < MAKS_P );
    
  if( lb_pojazdow == MAKS_P ) /* Czy jest miejsce w tablicy? */
  {
    printf( "\nEwidencja pelna, [Enter] >> powrot do menu." );
    ( void )getchar();
  }
}

/*----------------------------------------------------------------------------
  Przeglad pojazdow -- sekwencyjnie od pierwszego do ostatniego.
----------------------------------------------------------------------------*/
void pokaz_pojazdy( void )
{
  int nr;
  
  if( lb_pojazdow == 0 )
    printf( "\nEwidencja jest pusta." );
  
  for( nr = 0; nr < lb_pojazdow; nr++ )
  {
    przewin_ekran();
    printf( "\nDane pojazdu nr: %d\n", nr + 1 );
    pokaz_info( &pojazdy[ nr ] );
    if( nr < lb_pojazdow - 1 )
      printf( "\n\n[Enter] = Nastepny pojazd >>" );
    else
      printf( "\n\n[Enter] = Zakoncz przeglad" );
   ( void )getchar(); fflush( stdin );
  }
}

/*----------------------------------------------------------------------------
  Wyszukiwanie pojazdow w ewidencji. Szukanie po marce i maks. przebiegu.
----------------------------------------------------------------------------*/
void szukaj_pojazdu( void )
{
  int kryterium;

  printf( "\nSzukaj wg.:\n M - marki,\n P - przebiegu\n?>" );
  kryterium = tolower( getchar() );
  fflush( stdin );
  switch( kryterium )
  {
     case 'm' : szukaj_wg_marki();
                break;
     case 'p' : szukaj_wg_przebiegu();
                break;
  }
}

/*----------------------------------------------------------------------------
  Uswanie pojazdow z ewidencji. Usuwanie w trakcie sekwencyjnego przegl¹du.
----------------------------------------------------------------------------*/
void usun_pojazd( void )
{
  int nr;

  if( lb_pojazdow == 0 )
    printf( "\nEwidencja jest pusta." );

  for( nr = 0; nr < lb_pojazdow; nr++ )
  {
    przewin_ekran();
    printf( "\nDane pojazdu nr: %d\n", nr + 1 );
    pokaz_info( &pojazdy[ nr ] );
    
    printf( "\nCzy chcesz usunac ten pojazd? (t/n) ");
    if( tolower( getchar() ) == 't' )
    {
      fflush( stdin );
      printf( "Jestes tego pewien? (t/n) " );
      if( tolower( getchar() ) == 't' )
      {
        fflush( stdin );
        printf( "Usuwanie pojazdu %s...", &pojazdy[ nr ].nr_rej  );
        usun_z_ewidencji( nr );
      }
    }

    fflush( stdin );
    if( nr < lb_pojazdow - 1 )
      printf( "\n\n[Enter] = Nastepny pojazd >>" );
    else
      printf( "\n\n[Enter] = Zakoncz usuwanie" );
      
   ( void )getchar();
  }
}

/*----------------------------------------------------------------------------
  Usuniecie wszystkich pojazdow.
----------------------------------------------------------------------------*/
void usun_wszystko( void )
{
  if( lb_pojazdow == 0 )
    printf( "\nEwidencja jest pusta." );
  else
  {
    printf( "UWAGA! Czy rzeczywiscie chcesz usunac wyszystkie pojazdy? (t/n) ");
    if( tolower( getchar() ) == 't' )
    {
      fflush( stdin );
      printf( "Jestes tego pewnien? (t/n) " );
      if( tolower( getchar() ) == 't' )
      {
        fflush( stdin );
        unlink( nazwa_pliku );
        lb_pojazdow = 0;
      } 
    }
    fflush( stdin );
  }
}
/*----------------------------------------------------------------------------
  Odczyt zawartosci pliku do tablicy pojazdy. 
----------------------------------------------------------------------------*/
void z_pliku_do_tablicy( void )
{
  FILE * plik;
  
  printf( "\nLadowanie ewidencji..." );
  if( ( plik = fopen( nazwa_pliku, "rb" ) ) == NULL )
  {
    printf( "plik ewidencji pojazdow nie istnieje." );
    printf( "\nZakladam nowa, pusta ewidencje pojazdow..." );
    if( ( plik = fopen( nazwa_pliku, "wb" ) ) == NULL )
      printf( " blad tworzenia pliku." );
    else
    {
      fclose( plik );
      printf( " sukces." );
    }  
  }
  else
  {
    int n;
    n = fread( &lb_pojazdow, sizeof( lb_pojazdow ), 1, plik );
    if( n != 1  )
      printf( "\nBlad w pliku ewidencji pojazdow." );
    n = fread( pojazdy, sizeof( pojazd ), lb_pojazdow, plik );
    if( n != lb_pojazdow  )
      printf( "\nBlad w pliku ewidencji pojazdow." );
    fclose( plik );
  }  
}

/*----------------------------------------------------------------------------
  Zapis zawartosci tablicy pojazdy do pliku.
----------------------------------------------------------------------------*/
void z_tablicy_do_pliku( void )
{
  FILE * plik;
  
  printf( "\nZapisywanie ewidencji..." );
  if( ( plik = fopen( nazwa_pliku, "wb" ) ) == NULL )
    printf( " blad aktualizacji pliku." );
  else
  {
    int n;
    n = fwrite( &lb_pojazdow, sizeof( lb_pojazdow ), 1, plik );
    if( n != 1  )
      printf( "\nBlad zapisu pliku ewidencji pojazdow." );
    if( lb_pojazdow > 0 )
    {  
      n = fwrite( pojazdy, sizeof( pojazd ), lb_pojazdow, plik );
      if( n != lb_pojazdow  )
        printf( "\nBlad w pliku ewidencji pojazdow." );
    }
    fclose( plik );
  }  
}

/*----------------------------------------------------------------------------
  Wczytanie danych o pojezdzie ze standardowego strumienia wejsciowego do
  rekordu wskazywanego przez info.
----------------------------------------------------------------------------*/
void czytaj_info( pojazd * info )
{
  char bufor[ 128 ];
  
  printf( "\nPodaj dane pojazdu" );

  printf( "\nMarka: " ); 
  gets( bufor );
  if( strlen( bufor ) >= MAKS_M ) 
    bufor[ MAKS_M - 1 ] = '\0'; 
  strcpy( info->marka, bufor );

  printf( "Model: " ); 
  gets( bufor );
  if( strlen( bufor ) >= MAKS_M ) 
    bufor[ MAKS_M - 1 ] = '\0'; 
  strcpy( info->model, bufor );

  printf( "Rok produkcji: " ); 
  gets( bufor );
  info->rok_prod = atoi( bufor );

  printf( "Cena: " ); 
  gets( bufor );
  info->cena = atof( bufor );

  printf( "Przebieg: " ); 
  gets( bufor );
  info->przebieg = atof( bufor );

  printf( "Numer rejestracyjny: " ); 
  gets( bufor );
  if( strlen( bufor ) >= MAKS_R ) 
    bufor[ MAKS_R - 1 ] = '\0'; 
  strcpy( info->nr_rej, bufor );
}

/*----------------------------------------------------------------------------
  Wyprowadzenie informacji z rekordu wskazywanego przez info do stdout. 
----------------------------------------------------------------------------*/
void pokaz_info( const pojazd * info )
{
  printf( "\n      Marka: %s", info->marka );
  printf( "\n      Model: %s", info->model );
  printf( "\n  Rok prod.: %d", info->rok_prod );
  printf( "\n       Cena: %g", info->cena );
  printf( "\n   Przebieg: %g", info->przebieg );
  printf( "\nNr rejestr.: %s", info->nr_rej );
}

/*----------------------------------------------------------------------------
  Usuniecie pojazdu o indeksie nr z tablicy i przepisanie elementow.
----------------------------------------------------------------------------*/
void usun_z_ewidencji( int nr )
{
  if( nr >= 0 && nr < lb_pojazdow )
  {
    for( ; nr < lb_pojazdow - 1; nr++ )
      pojazdy[ nr ] = pojazdy[ nr + 1 ]; 
    lb_pojazdow--;  
  }    
}

/*----------------------------------------------------------------------------
  Wyszukiwanie wg. marki.
----------------------------------------------------------------------------*/
void szukaj_wg_marki( void )
{
  int nr, znaleziono = 0;
  char bufor[ 128 ];
  
  printf( "\nPodaj marke: " ); 
  gets( bufor );
 
  for( nr = 0; nr < lb_pojazdow; nr++ )
  {
    if( stricmp( pojazdy[ nr ].marka, bufor ) == 0  )
    {
      znaleziono = 1;
      printf( "\nDane pojazdu nr: %d\n", nr + 1 );
      pokaz_info( &pojazdy[ nr ] );
      if( nr < lb_pojazdow - 1 )
        printf( "\n\n[Enter] = Nastepny pojazd >>" );
      else
        printf( "\n\n[Enter] = Zakoncz przeglad" );
      ( void )getchar();
    } 
  }
  if( ! znaleziono )
  {
    printf( "\nBrak takich pojazdow\n[Enter] = Zakoncz przeglad" );
    ( void )getchar();
  }
}

/*----------------------------------------------------------------------------
  Wyszukiwanie wg. maksymalnego przebiegu.
----------------------------------------------------------------------------*/
void szukaj_wg_przebiegu( void )
{
  int nr, znaleziono = 0;
  float przebieg;
  char bufor[ 128 ];
  
  printf( "\nPodaj przebieg: " ); 
  gets( bufor );
  przebieg = atof( bufor );
  
  if( przebieg <= 0 || przebieg > 300000 )
  {
     printf( "\nNie szukam pojazdu dla takiego przebiegu." ); 
     return; 
  } 
  printf( "\nSzukam pojazdow o przebiegu nie wiekszym niz %g.", przebieg ); 
  
  for( nr = 0; nr < lb_pojazdow; nr++ )
  {
    if( pojazdy[ nr ].przebieg <= przebieg )
    {
      znaleziono = 1;
      printf( "\nDane pojazdu nr: %d\n", nr + 1 );
      pokaz_info( &pojazdy[ nr ] );
      if( nr < lb_pojazdow - 1 )
        printf( "\n\n[Enter] = Nastepny pojazd >>" );
      else
        printf( "\n\n[Enter] = Zakoncz przeglad" );
      ( void )getchar();
    } 
  }
  if( ! znaleziono )
  {
    printf( "\nBrak takich pojazdow\n[Enter] = Zakoncz przeglad" );
    ( void )getchar();
  }
}

/*----------------------------------------------------------------------------
  Przewin ekran o 25 linii. Prymitywna namiastka czyszczenia ekranu.
----------------------------------------------------------------------------*/
void przewin_ekran( void )
{
  int liczba_linii = 25;
  while( liczba_linii-- > 0 ) 
    putchar( '\n' ); 
}
