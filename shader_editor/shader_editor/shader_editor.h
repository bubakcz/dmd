#ifndef SHADER_EDITOR_H
#define SHADER_EDITOR_H
#define QT_DEBUG
#include <QtGui/QMainWindow>
#include "ui_shader_editor.h"
#include "ui_create_uniform.h"
#include "Include/AMD/GPUPerfAPI.h"
#include "test_repeater.h"

#include <osg/Notify>
#include <osg/Geometry>
#include <map>

class osgNotifyRedirect : public QObject, public osg::NotifyHandler
{
	Q_OBJECT
public:
	void notify(osg::NotifySeverity severity, const char *message)
	{
		emit writeToConsoleSignal(severity,message);
	};
signals:
	void writeToConsoleSignal(osg::NotifySeverity severity, const char *message);
};

// struct uniformsOfActualNode
// {
// 
// };

enum GPU_Vendor
{
	ATI,
	NVIDIA,
	NOT_SET
};

class shaderTab;
class SceneTreeManager;
class StateAttributeManager;

class shader_editor : public QMainWindow
{
	Q_OBJECT

public:
	shader_editor(QWidget *parent = 0, Qt::WFlags flags = 0);
	~shader_editor();
	void writeToConsoleQt(QtMsgType severity, const char *message);
	void writeToConsoleQt(QtMsgType severity, char *message);
	void writeToConsoleQt(QtMsgType severity, QString message );

	std::vector<QTableWidgetItem*> getItems_takePerfSnapshot_ATI();

	friend class shaderTab;
	friend class SceneTreeManager;
	friend class StateAttributeManager;

	std::map<int, int> perfUniversal_to_table;

protected:
	std::ofstream * osg_debug_output;
	GPU_Vendor gpuVendor;
	GPU_Vendor getVendorFromString(const GLubyte * glGetStringVendorRetVal);
	void timerEvent(QTimerEvent *event);

	shaderTab * shaderTabContainer;
	SceneTreeManager * sceneTreeContainer;
	StateAttributeManager * stateAttributeContainer;
	test_repeater * testRepeater;

	void loadExampleSimpleDisplacement_complexModel_normalMapping_switch(const char * fragment_shader);

	osg::Geometry * getAsteroidGeometry(int *vertexBindingValue, int *normalBindingValue, int *uvBindingValue, int *tangentBindingValue, int *bitangentBindingValue);

	void optimizeDemos(osg::Node * target);

	void loadExample_Cobblestone_SinglePart_Inner(const char * spacing = NULL, bool wireframe = false);

	void loadExample_Miscellaneous_Triangle_Inner(const char * shader_file_name);

	void loadExample_Miscellaneous_Quads_Inner(const char * shader_file_name);

	void lineCallFailure(int line, const char * file, const char * text);


private:
	enum consoleMode
	{
		chyba,
		info
	};
	void initShaderContent();
	bool loadShader(QPlainTextEdit * targetEdit, bool * modified, char * shaderType);
	bool loadShader(QPlainTextEdit * targetEdit, char * shaderType , char * filename);
	void saveAsShader(QPlainTextEdit * sourceEdit, bool * modified, char * shaderType);
	void saveShader(QPlainTextEdit * sourceEdit, bool * modified, char * shaderType);
	void saveAll();
	void saveAllFileExists(char * filename, QTextEdit sourceEdit);
	bool isCtrlPressed();
	void compileShaders(osg::Node * targetNode, bool tess, bool geom);
	void resetApplicationState();
	void initAttributeConvContainers();
	void initMap_uniformType_isEditable();
	void rebuildUniformsTable(osg::StateSet * this_set);
	void initNVAPI();



	std::string * uniformToString(osg::Uniform * uniformRead);
	bool stringToUniform(std::string * stringWrite, osg::Uniform * uniformWrite);
	std::string * uniformProtovalue(osg::Uniform::Type type);


	

	char * enumToStringGPA(int val);
	char * enumToStringGPA_UsageType(int val);
	char * enumToStringNV_UsageType(int val);

	void setupClean( bool noCamera = false);


	bool vertexModified;
	bool tessControlModified;
	bool tessEvallModified;
	bool geometryModified;
	bool fragmentModified;
	
	bool perfATI_initialized;
	bool perfATI_setup;
	bool perfNV_initialized;
	bool perfNV_setup;
	osgNotifyRedirect * notifyInstance;
	Ui::shader_editorClass ui;
	QWidget * cuwContainer;
	Ui::createUniformWidget cuw;

	std::map<osg::StateAttribute::Type, QString> attributeTypeToAttributeName;
	std::vector<osg::StateAttribute::Type> attributeTypeVector;
	std::map<osg::StateAttribute::Values, QString> attributeValuesToValuesName;
	std::map<QString, osg::StateAttribute::Values> valuesNamesToAttributeValues;
	QString attributeValuesTooltip;

	//std::map<int, int> perfUniversal_to_table;
	std::vector<bool> perfUniversal_table_colapsed;



	osg::Node * uniformTable_ActiveNode;

	std::map<osg::Uniform::Type,bool> uniformType_isEditable;
	std::map<osg::Uniform::Type,std::string> uniformType_name;
	bool uniformType_isEditable_mapSet;

	bool stopUniformTableValueChanged;

	osg::StateSet * actualStateSet;

	osg::Node * stateSetKeeper;

	int activeTimer;
	void appendToStateSetTable(osg::Node * this_node);
	

public slots:
	void QTE_vertexModified() {vertexModified=true;};
	void QTE_tessControlModified() {tessControlModified=true;};
	void QTE_tessEvallModified() {tessEvallModified=true;};
	void QTE_geometryModified() {geometryModified=true;};
	void QTE_fragmentModified() {fragmentModified=true;};
/*
	void QTE_vertexLoad() {loadShader(ui.textEdit_VS,&vertexModified,"Vertex Shader");};
	void QTE_tessControlLoad() {loadShader(ui.textEdit_TCS,&tessControlModified,"Tessellation Control Shader");};
	void QTE_tessEvalLoad() {loadShader(ui.textEdit_TES,&tessEvallModified,"Tessellation Evaluation Shader");};
	void QTE_geometryLoad() {loadShader(ui.textEdit_GS,&geometryModified,"Geometry Shader");};
	void QTE_fragmentLoad() {loadShader(ui.textEdit_FS,&fragmentModified,"Fragment Shader");};*/
	void displayDockSceneTree();
	void displayDockPerf();
	void displayDockConsole();
	void rebuildSceneTree();
	void rebuildUniformsTable(QTreeWidgetItem * this_item);
	void rebuildUniformsTable_fromStateSetTable(int row, int column);
	void rebuildStateSetTable();

	void rebuildAttributeList(osg::StateSet * this_set);
	void rebuildAttributesTable_StateSet(int row, int column);
	void rebuildAttributesTable_Node(QTreeWidgetItem * this_item);


	

	void takePerfSnapshot_ATI();
	void initPerfCounters_ATI();
	void hidePerfRow_ATI(int row, int column);

	void takePerfSnapshot_NV();
	void initPerfCounters_NV();
	void hidePerfRow_NV(int row, int column);

	void writeToConsole(osg::NotifySeverity severity, const char *message);	
	void compileShaders();
	void setCurrentSceneGraphWidgetItem(QTreeWidgetItem * item);//itemActivated ( QTreeWidgetItem * item, int column )
	void loadModel();
	void loadModelOptimalizer();
	void loadModel(QString filename, bool optimize);
	void saveMainSceneNode();
	
	void uniformTableValueChanged(int row);
	void uniformTableUniformRemove();
	void uniformTableUniformAdd();
	void uniformTableUniformAdd_cuwCreate();
	void uniformTableUniformAdd_cuwSetProtovalue();
	void statesetTableCreateNew();

	void treeWidgetSetStateSet();
	void treeWidgetAddNode();
	void treeWidgetDeleteNode();

	void loadExampleTeselace();
	//void loadExampleInitial();
	void loadExampleSimpleDisplacement();
	void loadExampleSimpleDisplacement_complexModel();
	void loadExampleSimpleDisplacement_complexModel_differentTesselation();
	void loadExampleSimpleDisplacement_normalMapping();
	void loadExampleSimpleDisplacement_normalMapping_2();
	void loadExampleSimpleDisplacement_complexModel_normalMapping();
	void loadExampleSimpleDisplacement_complexModel_normalMapping_normalMapProblem();
	void loadExampleSimpleDisplacement_complexModel_normalMapping_dynamicTessellation();
	void loadExampleSimpleDisplacement_complexModel_normalMapping_sewing();

	void loadExampleAsteroid_BasicLod();
	void loadExampleAsteroid_fromIVE();
	void loadExampleSceneTreeTest();
	void loadExampleVariableTesselation();

	void loadExampleLargeSkyboxScene();

	void loadExample_LOD_DynamicTess_16();
	void loadExample_LOD_DynamicTess_64();
	void loadExample_LOD_DynamicTess_16_with_Common_LOD();
	void loadExample_LOD_DynamicTess_64_with_Common_LOD();

	void loadExample_LOD_DynamicTess_16_with_Common_LOD_cust_range();
	void loadExample_LOD_DynamicTess_64_with_Common_LOD_cust_range();

	void loadExample_LOD_no_tessellation();
	void loadExample_LOD_no_tessellation_custom_range();

	void loadExample_Issues_Gaps_Blender();
	void loadExample_Issues_Gaps_MeshMapper();
	void loadExample_Issues_Sewing();
	void loadExample_Issues_UVs_Blender();
	void loadExample_Issues_UVs_MeshMapper();
	void loadExample_Issues_UVsWithManualTexture();

	void loadExample_Cobblestone_SinglePart();
	void loadExample_Cobblestone_MultiPart();

	void loadExample_Cobblestone_SinglePart_Gaps();
	void loadExample_Cobblestone_MultiPart_Gaps();

	void loadExample_Cobblestone_spacing_equal();
	void loadExample_Cobblestone_spacing_fractional_odd();
	void loadExample_Cobblestone_spacing_fractional_even();

	void loadExample_Cobblestone_spacing_equal_w();
	void loadExample_Cobblestone_spacing_fractional_odd_w();
	void loadExample_Cobblestone_spacing_fractional_even_w();

	void loadExample_Miscellaneous_Triangle();
	void loadExample_Miscellaneous_Triangle_Dynamic();
	void loadExample_Miscellaneous_Triangle_Dynamic_Regular();

	void loadExample_Miscellaneous_Quads();
	void loadExample_Miscellaneous_Quads_Dynamic();

	void loadExample_Performance_DynamicTessellation_Shader();
	void loadExample_Performance_DynamicTessellation_Callback();
	void loadExample_Performance_DynamicTessellation_Fixed();

	void loadExample_Performance_DynamicTessellation_Cobblestone_Combined();
	void loadExample_Performance_DynamicTessellation_Cobblestone_Shader();
	void loadExample_Performance_DynamicTessellation_Cobblestone_Shader_Modified();

	void loadExample_Performance_Tessellated();
	void loadExample_Performance_FullModel();

// 	void loadExample_Performance();
// 	void loadExample_Performance();
// 	void loadExample_Performance();
	

	void setTimerInterval(double d);
	void restartTimer(bool checked);

	void SA_Program_SetCurrentProgram();

	void massPerfCountersAnalyse();

	void clearScene();

	void makeGLWidgetFloating(bool checked);

	void setCamera1();
	void setCamera2();
	void setCamera3();
	void setCamera4();

	void setCameraMapped(int i);
};

#endif // SHADER_EDITOR_H






