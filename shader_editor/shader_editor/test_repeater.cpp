#include "test_repeater.h"
#include <QGridLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QRegExp>
#include <float.h>
#include "shader_editor.h"
#include <QTime>



test_repeater::test_repeater(QWidget * parent, QTableWidget * sourceTableWidget) : QObject()
{
	this->parent = parent;
	this->sourceTableWidget = sourceTableWidget;
	tableWidget = NULL;
	spinBox = NULL;
}


test_repeater::~test_repeater(void)
{
}

inline QString test_repeater::getCellNumber(int row, int col)
{
	QString tmpText = dataTable->item(row, col)->text();
	return tmpText.remove(QRegExp("(( ratio)|(%)|( cyklu)|(ms)|(kB)|(B)|( items)|(last))$"));
}

QTableWidgetItem * test_repeater::calculateMedianAndLowestForRow(int row, int rowLen, int counterIndex, QString * lowestNinja)
{
	shader_editor * parent = (shader_editor*)this->parent;
	QTableWidgetItem * newItem = new QTableWidgetItem;
	QString itemText;// = dataTable->item(row, 1)->text();


	GPA_Type dataType;
	GPA_GetCounterDataType( counterIndex, &dataType);

	std::size_t middle;

	if (dataType == GPA_TYPE_UINT32 || dataType == GPA_TYPE_UINT64)
	{
		std::vector<unsigned long> numberVector;
		for (int col = 0; col < rowLen; col++)
			numberVector.push_back(getCellNumber(row, col).toULong());
		middle = numberVector.size()/2;
		std::nth_element(numberVector.begin(), numberVector.begin()+middle,numberVector.end());
		itemText = QString::number(numberVector[middle]);
		if (lowestNinja)
		{
			unsigned long min = ULONG_MAX;
			for (int i = 0; i < numberVector.size(); i++)
				if (numberVector[i] < min)
					min = numberVector[i];
			*lowestNinja = QString::number(min);
		}
	}
	else
	{
		std::vector<double> numberVector;
		for (int col = 0; col < rowLen; col++)
			numberVector.push_back(getCellNumber(row, col).toDouble());
		middle = numberVector.size()/2;
		std::nth_element(numberVector.begin(), numberVector.begin()+middle,numberVector.end());
		itemText = QString::number(numberVector[middle]);
		if (lowestNinja)
		{
			double min = DBL_MAX;
			for (int i = 0; i < numberVector.size(); i++)
				if (numberVector[i] < min)
					min = numberVector[i];
			*lowestNinja = QString::number(min);
		}
	}

	//parent->writeToConsoleQt(QtWarningMsg, QString("row: ")+QString::number(row));


	newItem->setText(itemText);

	return newItem;
}

/*	

loadModel(filename, false);

int difference = time.elapsed();
QString text; 
writeToConsoleQt(QtDebugMsg, text.sprintf("Otevirani souboru trvalo: %02d:%02d.%03d ms",difference/60000,(difference%60000)/1000,difference%1000));*/

void test_repeater::run()
{
	QTime time;
	time.start();
	shader_editor * parent = (shader_editor*)this->parent;

	QWidget * window = get_table();
	window->hide();

	int cycles = spinBox->value();

	dataTable->setColumnCount(cycles+1/*Median*/+1/*Lowest*/);

	int sane_check = -1;

	for (int j = 0; j < cycles ; j++)
	{
		std::vector<QTableWidgetItem*> itemVector = parent->getItems_takePerfSnapshot_ATI();
		if (sane_check >= 0 && itemVector.size() != sane_check)
			parent->writeToConsoleQt(QtWarningMsg, QString("test_repeater::run(): itemVector.size() != sane_check shouln't happen"));
		for (int i = 0; i < itemVector.size(); i++)
		{
			dataTable->setItem(parent->perfUniversal_to_table[i], j , itemVector[i]);
		}
		sane_check = itemVector.size();
		/*if (j>0 && j%10 == 0)
			((shader_editor*)this->parent)->writeToConsoleQt(QtDebugMsg,QString("test_repeater: dokoncen cyklus ")+QString::number(j));*/
	}
	//median
	for (int i = 0; i < sane_check; i++)
	{
		int row = parent->perfUniversal_to_table[i];
		QString lowest;
		dataTable->setItem(row, cycles, calculateMedianAndLowestForRow(row, cycles, i, &lowest));
		dataTable->setItem(row, cycles+1, new QTableWidgetItem(lowest));
	}
	dataTable->setHorizontalHeaderItem(cycles,new QTableWidgetItem("Median"));
	dataTable->setHorizontalHeaderItem(cycles+1,new QTableWidgetItem("Lowest"));




	window->show();

	int difference = time.elapsed();
	QString text; 
	parent->writeToConsoleQt(QtDebugMsg, text.sprintf("Celé mìøení trvalo: %02d:%02d.%03d ms",difference/60000,(difference%60000)/1000,difference%1000));

}


/*unsigned int test_repeater::run_dialog()
{
	return get_dialog()->exec() == QDialog::Accepted ? get_dialog()->intValue() : 0 ;
}*/

void test_repeater::run_table()
{
	QWidget * window = get_table();
	if (window->isHidden())
		window->show();
}
/*
QInputDialog * test_repeater::get_dialog()
{
	if (in_dialog)
		return in_dialog;

	static const int range_min = 1;
	static const int range_max = 1000;

	in_dialog = new QInputDialog(parent, Qt::Dialog);
	in_dialog->setIntMaximum(range_max);
	in_dialog->setIntMinimum(range_min);
	in_dialog->setInputMode(QInputDialog::IntInput);
	in_dialog->setLabelText(QString("Poèet opakování mìøení"));
	in_dialog->setCancelButtonText(QString("Vynechat mìøení"));

	return in_dialog;
}*/

QWidget * test_repeater::get_table()
{
	if (tableWidget)
	{
		return tableWidget;
	}

	tableWidget = new QWidget();
	//tableWidget->setLayout(new QGridLayout(tableWidget));

	QGridLayout * layout = new QGridLayout();//(QGridLayout *)tableWidget->layout();

	layout->setSpacing(6);
	layout->setContentsMargins(0, 0, 0, 6);

	dataTable = new QTableWidget(sourceTableWidget->rowCount(),1, tableWidget);
	dataTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//dataTable->horizontalHeader()->setStretchLastSection(true);
	dataTable->setMinimumWidth(800);
	dataTable->setMinimumHeight(400);
	dataTable->setMaximumHeight(4096);
	dataTable->setMaximumWidth(8192);

	layout->addWidget(dataTable,0,0,1,4/*,Qt::AlignCenter*/);
	layout->setAlignment(dataTable, Qt::AlignLeft | Qt::AlignTop);

	spinBox = new QSpinBox();
	spinBox->setMinimum(1);
	spinBox->setMaximum(1000);
	spinBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	layout->addWidget(spinBox,1,0,Qt::AlignCenter);
	spinBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

	QPushButton * analyse = new QPushButton(QString("Odecist"), tableWidget);
	analyse->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	layout->addWidget(analyse,1,1,Qt::AlignCenter);

	bool connectRetval = true;

	connectRetval &= connect(analyse,SIGNAL(clicked()),this,SLOT(run()));



	QPushButton * showLatexTabular = new QPushButton(QString("Zobrazit latex code (oznacene radky)"), tableWidget);
	showLatexTabular->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	layout->addWidget(showLatexTabular,1,2,Qt::AlignCenter);

	connectRetval &= connect(showLatexTabular,SIGNAL(clicked()),this,SLOT(showLatexCode()));

	QPushButton * showCSV = new QPushButton(QString("Zobrazit csv"), tableWidget);
	showCSV->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	layout->addWidget(showCSV,1,3,Qt::AlignCenter);

	connectRetval &= connect(showCSV,SIGNAL(clicked()),this,SLOT(showCSV()));

	if (!connectRetval)
	{
		((shader_editor*)this->parent)->writeToConsoleQt(QtCriticalMsg,QString("test_repeater: connect failed"));
	}

	layout->setSizeConstraint(QLayout::SetMaximumSize);

	layout->setRowStretch(0,1);
	layout->setRowStretch(1,0);

	tableWidget->setLayout(layout);

	for (int i = 0; i < sourceTableWidget->rowCount(); i++)
	{
		if (sourceTableWidget->item(i,0))
			dataTable->setVerticalHeaderItem(i,new QTableWidgetItem(*sourceTableWidget->item(i,0)));//setItem(i,0,new QTableWidgetItem(*sourceTableWidget->item(i,0)));
	}


	return tableWidget;
}

bool skipRow(QTableWidgetItem * item)
{
	if (item == NULL)
		return true;

	if (item->text().at(0) == '+')
		return true;
	return false;
}

void test_repeater::showLatexCode()
{
	QTextEdit * window = new QTextEdit();
	window->setWindowTitle(QString("Tabulka v latex kodu"));
	window->append(QString("{\\centering"));
	window->append(QString("  \\begin{table}[H]"));
	//window->append(QString("    \\begin{tabular}{ | c |"));
	QString line = "    \\begin{tabular}{ | c |";
	for (int i = 1; i < dataTable->columnCount(); i++)
	{
		line += " c |";// ""//window->append(QString(" c |"));
	}
	window->append(line);
	window->append(QString("    \\hline"));
	QList<QTableWidgetItem *> selectedItems = dataTable->selectedItems();
	bool * process = new bool[dataTable->rowCount()];

	if (selectedItems.size())
	{
		for ( int z = 0; z < dataTable->rowCount(); z++) process[z]=false;
		for ( int z = 0; z < selectedItems.size(); z++) 
			if (selectedItems[z]->row() < dataTable->rowCount())
				process[selectedItems[z]->row()] = true;
	
	}
	else
		for ( int z = 0; z < dataTable->rowCount(); z++) process[z]=true;

	line.clear();
	for (int i = 1; i < dataTable->columnCount(); i++)
	{
		if(dataTable->horizontalHeaderItem(i) != NULL)
			line += QString(" & ")+dataTable->horizontalHeaderItem(i)->text();
		else
			line += QString(" & ")+QString::number(i);
	}
	line+= " \\\\";
	window->append(line);	
	
	for (int k = 0; k < dataTable->rowCount(); k++)
	{
		line.clear();
		if(!skipRow(dataTable->item(k,0)) && process[k])
		{
			if(dataTable->verticalHeaderItem(k) != NULL)
			{
				line += dataTable->verticalHeaderItem(k)->text();
				line += " & ";
			}
			for (int i = 0; i < dataTable->columnCount(); i++)
			{
				if(dataTable->item(k,i) != NULL)
				{
					line += dataTable->item(k,i)->text();
					if (i + 1 < dataTable->columnCount())
						line += " & ";
				}

			}
			line+= " \\\\";
			window->append(line);
		}
	}
	window->append(QString("    \\hline"));
	window->append(QString("    \\end{tabular}"));
	window->append(QString("    \\caption{sem vlozit text}"));
	window->append(QString("  \\end{table}"));
	window->append(QString("}"));

	window->setWindowModality(Qt::WindowModal);

	window->show();


		/*{\centering
		\begin{table}[H]
		\begin{tabular}{ | c | c | c | }
		\hline
		& Polygonù & Celková pamì \\ \hline
		Nízké rozlišení modelu s teselací & 5,160 & {Vertex a index buffer: 100KB } \\ 
		& & 2K x 2K 16b displacement map: 10MB \\ \hline
		Vysoké rozlišení modelu & 15M+ & {Vertex buffer: 270MB } \\
		& &  Index buffer: 180MB \\
		\hline
		\end{tabular}
		\caption{Velikost redukovaného modelu froblina a pùvodního}
		\end{table}
		}*/
}

void test_repeater::showCSV()
{
	QTextEdit * window = new QTextEdit();

	window->setWindowTitle(QString("Tabulka v CSV podobe"));

	QString line;

	for (int i = 1; i < dataTable->columnCount(); i++)
	{
		if(dataTable->horizontalHeaderItem(i) != NULL)
			line += QString(" , ")+dataTable->horizontalHeaderItem(i)->text();
		else
			line += QString(" , ")+QString::number(i);
	}
	window->append(line);	

	for (int k = 0; k < dataTable->rowCount(); k++)
	{
		line.clear();
		if(!skipRow(dataTable->item(k,0)))
		{
			if(dataTable->verticalHeaderItem(k) != NULL)
			{
				line += dataTable->verticalHeaderItem(k)->text();
				line += " , ";
			}
			for (int i = 0; i < dataTable->columnCount(); i++)
			{
				if(dataTable->item(k,i) != NULL)
				{
					line += dataTable->item(k,i)->text();
					if (i + 1 < dataTable->columnCount())
						line += " , ";
				}

			}
			window->append(line);
		}
	}

	window->show();
}

