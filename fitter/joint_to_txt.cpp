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
#include "BExtractedCrossTalk.h"
#include "BExtractedCrossTalkTel.h"
#include "BExtractedHeader.h"
#include "BExtractedImpulse.h"
#include "BExtractedImpulseTel.h"
#include "BExtractedIter.h"
#include "BExtractor.h"
#include "BImpulseIncl.h"
#include "BImpulseLinkDef.h"
#include "BJoinExtractedImpulseTel.h"
#include <fstream>
#include <sstream>
#include <string>

int joint_to_txt(TString input_path = "k0362.joint.events.marked.root", 
				 TString output_directory = "out", 
				 int THRESHOLD = 10000)
{
	int z=0, r=0;
	ofstream txt;
	double circ=200.,circ_hard=20.;

	TString PathToRawEvents = Form(input_path); //строка с именем читаемого файла
	TFile FileRawEvents(PathToRawEvents); //создаём файловую переменную читаемого файла

	stringstream output_path_stream;
	output_path_stream << output_directory << "/thres" << THRESHOLD << ".txt";
	TString output_path = output_path_stream.str();

//проверяем, открылся ли файл
	if (!FileRawEvents.IsOpen()) {
		cout << "ERROR - Could not find input root file: " << PathToRawEvents << endl;
	}
	

// Записываем дерево Events в переменную RawTree:
	TTree *RawTree = (TTree*)FileRawEvents.Get("Events");
// Создаем перменную JointImpulseTel из класса BExtractedImpulseTel:
	BExtractedImpulseTel *JointImpulseTel = new BExtractedImpulseTel();
// Связываем переменную JointImpulseTel с веткой BJointImpulseTel:
	RawTree->SetBranchAddress("BJointImpulseTel", &JointImpulseTel);

	txt.open(output_path); //создаём и открываем файл записи

//проверяем был ли открыт файл записи
	if (!txt.is_open()) {
		cout << "Could not open output .txt file by path: " << output_path << endl;
	}
	txt << "j	" << "nch	" << "z	" << "T	" << "Q" << endl;

//цикл по всем событиям файла (не путать с Events, событием далее называется ветка дерева)
//в каждом событии находится вся информация информация по конкретной временной дорожке длинной 1024 АЦП
	for (int j = 0; j < RawTree->GetEntries(); j++){
		RawTree->GetEntry(j);
		Float_t max=0.,Tsr = 0.;
		Int_t nchmax = 0;
		int upper = 0;
		int lower = 0;
//цикл определения максимального значения заряда, зафиксированного в этом событии
//записываем соответствующий канал, среднее время относительно максимума
		for (int k=0 ; k < JointImpulseTel->GetNimpulse(); k++) {
			Float_t Qt = JointImpulseTel->GetQ(k);
			if (Qt > max) {
				max = Qt;
				nchmax = JointImpulseTel->GetNch(k);
				Tsr = JointImpulseTel->GetT(k);
			}
		}
		int str = int(nchmax/36)+1; //номер стринга, на котором был зафиксирован Qmax
		upper = 36*str; //верхний канал стринга
		lower = upper - 36; //нижний канал стринга
//вводим первичное ограничение, чтобы избавиться от мусорных данных (порог обсуждается)
		if (max>THRESHOLD) {
//цикл по всем импульсам события
			cout << "Processing event " << j << "\n";
			r=0;
			for (int q=0 ; q < JointImpulseTel->GetNimpulse(); q++) {
				Int_t nch_prob = JointImpulseTel->GetNch(q);
				Float_t Q_prob = JointImpulseTel->GetQ(q);
				Float_t T_prob = JointImpulseTel->GetT(q);
				if ((nch_prob>=lower)&&(nch_prob<upper)&&(Q_prob>0)&&(T_prob>=Tsr-circ_hard)&&(T_prob<=Tsr+circ_hard)) r+=1;
			}
			if (r>5) {
			for (int i = 0; i < JointImpulseTel->GetNimpulse(); i++) {
				Int_t nch = JointImpulseTel->GetNch(i);
				Float_t Q = JointImpulseTel->GetQ(i);
				Float_t T = JointImpulseTel->GetT(i);
//вводим дополнительные ограничения для просмотра нужного стринга и нужного отрезка времени
				if ((nch>=lower)&&(nch<upper)&&(Q>0)&&(T>=Tsr-circ)&&(T<=Tsr+circ)) {
					z+=(nch-lower)*15; //грубое определение глубины канала (0>z<540)
					//выводим ключерые данные в терминал
					txt << j << "	" << nch << "	" << z << "	" << T << "	" << Q << endl;
//					cout << "j=" << j << " i=" << i << " nch=" << nch << " Q=" << Q << " T=" << T << endl;
					z=0; //обнуление перед концом цикла
					}
				}
			}
			}
		}
		txt.close(); //закрываем файл записи
		cout << "Data written to " << output_path << "\n";
		return 1;
	}
