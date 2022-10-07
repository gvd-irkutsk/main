#include <cmath> // Стандартная математическая библиотека Си++
#include <fstream> // Стандартная библиотека для ввода-вывода в файл
#include <TMath.h> // Математическая библиотека root
#include <TMultiGraph.h>
#include "TFile.h" // Библиотека root для работы с файлами
#include "TTree.h" // Библиотека root для создания древесных структур
#include "BRawMasterData.h" // Библиотека bars со стандартными функциями для работы с сырыми данными. Лежит в папке /bars/braw
#include "BRawMasterHeader.h"
#include "TGraphErrors.h" // Библиотека root для построения графиков с погрешностями
#include "TH1F.h" // Библиотека для одномерных функций
#include "TCanvas.h" // Холст
#include "TLegend.h" // Подписи на графиках

int integral()
{
	Int_t Season = 2021;
	Int_t cluster = 6;
	Int_t RunNumber = 343;
	Int_t fileNum = 2;
	Int_t Sdc = 192; // Можно выбирать от 192 до 215
	int t=0;
	int y=1;
	FILE *f0,*f1,*f2,*f3,*f4,*f5,*f6,*f7,*f8,*f9,*f10,*f11;
	TString PathToRawEvents = Form("/home/user/gvd/DATA/%04d/k0343.raw.events.%d.%04d.root", fileNum, Sdc, fileNum);
		TFile FileRawEvents(PathToRawEvents);
		
	TString fOutWaveFileName = Form("/home/user/gvd/DATA/results/forStudents_%d_%04d.root", Sdc, fileNum);
		TFile fWaveout(fOutWaveFileName.Data(), "recreate");	
// Проверяем, открылся ли файл с данными		
				if (!FileRawEvents.IsOpen()) {
					cout << "ERROR - Could not find file: " << PathToRawEvents << endl;
					}
// Записываем дерево Events в переменную RawTree:
				TTree *RawTree = (TTree*)FileRawEvents.Get("Events");
// Создаем перменную rawMasterData из класса BRawMasterData:			
				BRawMasterData *rawMasterData = new BRawMasterData();
// Связываем переменную rawMasterData с веткой BRawMasterData:				
				RawTree->SetBranchAddress("BRawMasterData.", &rawMasterData);

// Создаем еще одну ветку (в ней время событий):
				BRawMasterHeader *rawMasterHeader = new BRawMasterHeader();
				RawTree->SetBranchAddress("BRawMasterHeader.", &rawMasterHeader);	

double sum=0.;
//создаём 12 файлов для дальнейшей записи туда значений заряда для каждого канала (существут до момента, пока я не пойму, как сделать переменное имя файла)
f0=fopen("integral0.txt","w");  
f1=fopen("integral1.txt","w");
f2=fopen("integral2.txt","w");
f3=fopen("integral3.txt","w");
f4=fopen("integral4.txt","w");
f5=fopen("integral5.txt","w");
f6=fopen("integral6.txt","w");
f7=fopen("integral7.txt","w");
f8=fopen("integral8.txt","w");
f9=fopen("integral9.txt","w");
f10=fopen("integral10.txt","w");
f11=fopen("integral11.txt","w");
			TMultiGraph *mg = new TMultiGraph(); // задаем переменную типа мультиграф, для построения всех графиков на одном холсте 
			
// Цикл по всем событиям(=строкам иногда, иногда строк больше) в файле (функция GetEntries возвращает количество записей в файле):		
			for (int j = 0; j < RawTree->GetEntries(); j++){
// В зависимости от целей ограничиваем j либо каким-то константным значением, либо RawTree->GetEntries(), тогда охватываются все имеющиеся данные			
				RawTree->GetEntry(j);
				
				//cout<<" Количество импульсов в "<<j<<"событии = "<<rawMasterData->GetNumSamples()<<endl;
				//Double_t EventTime = rawMasterHeader->GetTimeVME(); это время самого события, оно нам не нужно, т.к. нам нужно время импульса в течение одного события
// Цикл по всем импульсам всех каналов в одном событии
				
				for (int i = 0; i < rawMasterData->GetNumSamples(); i++) {
		// Объект sample, в котором сидит вся информация по конкретному импульсу (амплитуда, время, номер канала...)
					BRawFADCSample *sample = rawMasterData->GetFADCSample(i);
					Int_t nch = sample->GetNch();		// Считываем номер канала (эти функции описаны в заголовочном файле bars/braw/BRawMasterData.h)
					Int_t nbins = sample->GetNbins();	// Считываем длину импульса
					Short_t *data = sample->GetData();  // (Массив из амплитуд) Записываем значения fData по этому импульсу на этом канале в переменную data
					Int_t offset = sample->GetOffset(); // Считываем задержку, компенсирующую длину кабеля
					if (nch==0) {  
//выбор канала для которого мы рисуем форму импульса. При этом заполняется только один из 12 файлов (если требуются только значения заряда, то это условие можно исключить)
					
// Вычисление пьедестала амплитуды как среднего арифметического первых 10ти точек в амплитуде:					
					double pedestal = 0;
					const int c_nPed = 8; // in BExtractor 10 counts, Lukas uses 5 counts
					for (int k = 0; k < c_nPed; k++) {
									pedestal += data[k];
								}
					pedestal /= c_nPed;
// Построение графика - зависимость амплитуды импульса от времени на текущем канале					
					TGraphErrors *WaveFormGraph = new TGraphErrors();
					WaveFormGraph->SetName(Form("%d_waveform_chan%d_Event%d_impulse%d_cl%d",t, nch, j, i, cluster));
					WaveFormGraph->SetTitle(Form("waveform_chan%d_Event%d_impulse%d_cl%d", nch, j, i, cluster));
			// Цикл по отсчетам АЦП от 0 до длины импульса в кодах АЦП
					for(Int_t n = 0; n < nbins; n++) {
						if (n>60) break;
						//if ((data[n]-pedestal)<(-20)) break;   //когда нужно лучше рассмотреть какую-то область на графике
						//if ((data[n]-pedestal)>20) break;   //то же самое
						if ((n+1)>=nbins) break; //так как в подсчёте sum мы используем data[n+1], то эта строчка нужна, чтобы мы не вышли за значения массива
						WaveFormGraph->SetPoint(n, n, data[n]-pedestal); // рисуем форму импульса по точкам
						sum+=(data[n]+data[n+1]-2*pedestal)*(1./2.);  //подсчёт интегралов
					}
					if (nch==0) fprintf(f0,"%f\n",sum);  //записываем вычесленный заряд в ранее созданные файлы
					if (nch==1) fprintf(f1,"%f\n",sum);
					if (nch==2) fprintf(f2,"%f\n",sum);
					if (nch==3) fprintf(f3,"%f\n",sum);
					if (nch==4) fprintf(f4,"%f\n",sum);
					if (nch==5) fprintf(f5,"%f\n",sum);
					if (nch==6) fprintf(f6,"%f\n",sum);
					if (nch==7) fprintf(f7,"%f\n",sum);
					if (nch==8) fprintf(f8,"%f\n",sum);
					if (nch==9) fprintf(f9,"%f\n",sum);
					if (nch==10) fprintf(f10,"%f\n",sum);
					if (nch==11) fprintf(f11,"%f\n",sum);
					sum=0.;  //обнуляем переменную для обновления подсчёта
					fWaveout.cd(); //начинаем запись в root файл
					//WaveFormGraph->Write(); // Запись графика в файл
					mg->Add(WaveFormGraph); //добавляем отдельный график на общий холст
					
				  } else break;  //конец выбора канала
				  
			}
		}
		mg->GetXaxis()->SetTitle("time, FADC counts"); //подпись осей
			    mg->GetYaxis()->SetTitle("amplitude, FADC counts");
				mg->Draw("al"); //рисуем все графики, добавленные на общий холст
				mg->Write();
fWaveout.Close(); //закрываем  root файл, в который записывали
fclose(f0);  //закрываем все наши файлы
fclose(f1);
fclose(f2);
fclose(f3);
fclose(f4);
fclose(f5);
fclose(f6);
fclose(f7);
fclose(f8);
fclose(f9);
fclose(f10);
fclose(f11);

 TH1F* h1 = new TH1F("h1","integral;Charge, FADC;Number of signals",200,0,300);
  ifstream infile;
  infile.open("integral0.txt");
  double tmp;
  FILE *f12;
  TString fOutWaveFileName1 = Form("/home/user/gvd/DATA/results/histogram_%d_%04d.root", Sdc, fileNum);
		TFile fWaveout1(fOutWaveFileName1.Data(), "recreate");	
	TF1 * f = new TF1("f","gaus",0,300);
	f->SetParameter(0, 360);
	f->SetParameter(1, 93.14);
	f->SetParameter(2, 34.85);
  while (infile>>tmp) {
	  //printf("%f\n",tmp);
      h1->Fill(tmp,2.5);
  }
  fWaveout1.cd();
  h1->Draw();
  h1->Fit("f","c");
  h1->Write();
  double mean = f->GetParameter(1);
  double sigma = f->GetParameter(2);
  cout << mean << endl;
  f12=fopen("charge1.txt","a+");
  fprintf(f12,"%f\n",mean);
  fclose(f12);
  fWaveout1.Close();
return 0;
}
