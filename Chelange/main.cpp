#include<string>
#include<stdio.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include<iostream>
#include<vector>
#include<math.h>
#include<fstream>
using namespace std;
//http://dmilvdv.narod.ru/SpeechSynthesis/fir.html {0.7125,-0.287808,-0.12725,0.09218922,0,0.0221922,-0.90625,-0.47808};0.289808  {0.7125,-0.289808,-0.12725,0.09218922,0,0.0221922,-0.90625,-0.47808};
void Filter (vector<int> in, double out[], int sizeIn){
const int N = 8; //Длина фильтра 117    (5,24) last {0.7125,-0.287808,-0.12625,0.0921922,0,0.0221922,-0.90625,-0.47808};12725
long double Fd = 476; //Частота дискретизации входных данных
//{0.91875,-0.258831,0.110198,-0.077254,-0.040625,0.2113907,-0.12135,-0.0128248,0,-0.0225248,-0.0165517,-0.0223907,-0.240625,-0.067254,0.110198,-0.258831}
long double H [N] = {0}; //Импульсная характеристика фильтра  0.925625,-0.0551306,-0.022981,-0.00490871,-1.38778e-19
long double H_id [8] = {0.7125,-0.287808,-0.12725,0.09218922,0,0.0221922,-0.90625,-0.47808}; //Идеальная импульсная характеристика0.920625,-0.271855,0.172375,-0.059945,-0.061875,0.056845,-0.059375,-0.070455
//long double H_id [8]={0.9125,-0.617808,-0.12725,-0.08218922,0,0.0221922,-0.90625,-0.97808};
long double W [N] = {0}; //Весовая функция

//Расчет импульсной характеристики фильтра
for (int i=0;i<N;i++)
{
// весовая функция Блекмена
 if (i<N/2) W [i] = 0.42 - 0.5 * cosl((2*M_PI*i) /( N-1)) + 0.08 * cosl((4*M_PI*i) /( N-1));
 else W [i] = 0;
H [i] = H_id[i] * W[i];
}
//Нормировка импульсной характеристики
double SUM=0;
for (int i=0; i<N; i++) SUM +=H[i];
for (int i=0; i<N; i++) H[i]/=SUM; //сумма коэффициентов равна 1
//Фильтрация входных данных
for (int i=0; i<sizeIn; i++)
{
out[i]=0.;
for (int j=0; j<N-1; j++)// формула фильтра
if(i-j>=0)
out[i]+= H[j]*in[i-j];

  if (i>100){
           out[i]=(out[i]+0.5*out[i-1]+0.13*out[i-2])/1.63;  // экспоненциальный фильтр для уменньшения вклада шумов
            }
}
}

int main()
{  int mn;
   int l,m;
   int i=0;
   int av=0;
   vector<int> mas[13] ;
    ifstream fin;
    fin.open("/home/matey/1_1_a.csv"); // выбор входных данных   2_8_a
    string b;
    int g,p,r;
if(!fin){
cout<<"No such file"<<endl;
}

//  ввод данных

while(!fin.eof()){
    getline(fin, b, ',');
    g=stoi(b);
    if(i>30&&(i%12)>1){
  g=(g+0.5*p+0.13*r)/1.63;
  if (g>mn) mn=g;
   }
    mas[i%12].push_back(g);
    r=p;
    p=g;
    i++;
}
    int G=1+(i-(i%12))/12;
    cout<<G<<endl;
    double output[13][G];


    for(int i=0;i<12;i++)Filter(mas[i],output[i],G);

// фильтр изображения 3х3 и вычисление среднего
av=output[0][0];
for(int ct=0;ct<13;ct++){
for (int f=0; f<12;f++){
    for(int j=0; j<mas[f].size();j++)
    {

    if((j>3)&&(j<mas[f].size())&&(f>0)&&(f<11)){
        if(abs(output[f][j-1]+output[f][j+1]+output[f-1][j]+output[f+1][j]-4*output[f][j])>1000)
                                {
                              output[f][j]=(output[f-1][j]+output[f+1][j]+output[f][j-1]+output[f][j+1]+output[f-1][j+1]+output[f+1][j-1]+output[f-1][j-1]+output[f+1][j+1])/8;
                                }

        }
     if(j>(-1)) av=(av+output[f][j])/2;
    }
}
}
     cout<< av<<" "<<mn<<endl;
    fin.close();
    // обьявляю используемые фигуры
    sf::RectangleShape shape(sf::Vector2f(2,2));
    sf::VertexArray qube(sf::Triangles, 4);
    sf::VertexArray ln(sf::Lines, 2);
    sf::VertexArray ram(sf::Quads, 4);

    // создание окна
 sf::RenderWindow window(sf::VideoMode(800, 6000), "My window");

 // программа работает столько, сколько открыто окно
 while (window.isOpen())
 {
 // проверяем все события окна которые сработали с момента последней итерации цикла
 sf::Event event;
 while (window.pollEvent(event))
 {
 // событие "закрытия": мы закрываем окно
 if (event.type == sf::Event::Closed)
 window.close();
 }

 window.clear(sf::Color::White);

int df=av;
if (abs(av)>10000) df=av/10;
//  задаю цвета элементов отображения
    ln[0].color=sf::Color::Black;
    ln[1].color=sf::Color::Black;
    ram[0].color=ram[1].color=ram[2].color=ram[3].color=sf::Color::White;


 for(int h=0;h<12;h++){
 for(int j=0; j<mas[0].size();j++)
    {
    if(abs(av-output[h][j])<0){
    shape.setFillColor(sf::Color(250,250,250,230));
       }
    else{
    if (output[h][j]>av){//lkz gthfjuj afqkf 0.05 and 18
    qube[0].color=sf::Color(255-0.05*abs(output[h][j+1]-av), 0, 0,255*abs(output[h][j+1]-av)/abs(18*df));
    qube[3].color=sf::Color(255-0.05*abs(output[h][j+1]-av), 0, 0,255*abs(output[h][j+1]-av)/abs(18*df));
                if(h<11){
                    qube[1].color=sf::Color(255-0.05*abs(output[h][j+1]-av), 0, 0,255*abs(output[h][j+1]-av)/abs(18*df));
                    qube[2].color=sf::Color(255-0.05*abs(output[h][j+1]-av), 0, 0,255*abs(output[h][j+1]-av)/abs(18*df));

                    }
     }
     else{

                qube[0].color=sf::Color(0,0, 255-0.05*abs(output[h][j+1]-av),255*abs(output[h][j+1]-av)/abs(18*df));
                qube[3].color=sf::Color(0, 0, 255-0.05*abs(output[h][j+1]-av),255*abs(output[h][j+1]-av)/abs(18*df));
                if (h<11){
                                qube[1].color=sf::Color(0, 0, 255-0.05*abs(output[h][j+1]-av),255*abs(output[h][j+1]-av)/abs(18*df));
                                qube[2].color=sf::Color(0, 0, 255-0.05*abs(output[h][j+1]-av),255*abs(output[h][j+1]-av)/abs(18*df));
                         }
     };
  // задем позицию элемента
  // в каждом стобце по 500 точек
    l=100+2*h+50*int(j/500);i+26,
    m=2*(j%500);
    qube[0].position = sf::Vector2f(l, m);
    qube[1].position = sf::Vector2f((l+4), m);
    qube[2].position = sf::Vector2f((l+4), (m+4));
    qube[3].position = sf::Vector2f(l, (m+4));
    }
     window.draw(qube);
    }
}

for(i=100;i<400;i+=50){
ram[0].position=sf::Vector2f(i+24,0);
ram[1].position=sf::Vector2f(i+26,0);
ram[2].position=sf::Vector2f(i+26,1002);
ram[3].position=sf::Vector2f(i+24,1002);
window.draw(ram);
ram[0].position=sf::Vector2f(i,0);
ram[1].position=sf::Vector2f(i+2,0);
ram[2].position=sf::Vector2f(i+2,1002);
ram[3].position=sf::Vector2f(i,1002);
window.draw(ram);
}
for(i=0;i<1000;i+=200){
ln[0].position=sf::Vector2f(0, i);
ln[1].position=sf::Vector2f(2000, i);
window.draw(ln);
}
 window.display();
 }
}
