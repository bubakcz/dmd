#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QInputDialog>
#include <QSpinBox>

class test_repeater : public QObject
{
	Q_OBJECT
public:
	test_repeater(QWidget * parent, QTableWidget * sourceTableWidget);
	~test_repeater(void);

	void run_table();

protected:

	//unsigned int run_dialog();



	//QInputDialog * get_dialog();

	QTableWidgetItem * test_repeater::calculateMedianAndLowestForRow(int row, int rowLen, int counterIndex, QString * lowestNinja);

	inline QString test_repeater::getCellNumber(int row, int col);

	QWidget * get_table();

	QWidget * parent;

	//QInputDialog * in_dialog;

	QWidget * tableWidget;

	QTableWidget * sourceTableWidget;

	QTableWidget * dataTable;

	QSpinBox * spinBox;

public slots:

	void run();

	void showLatexCode();

	void showCSV();

};

