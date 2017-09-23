
#include <iostream>
#include <qtimer.h>
#include <QApplication>
#include <QMessageBox>
#include <QQmlContext>

#include "../controller/modconfig.h"
#include "paths.h"
#include "xmui.h"
#include "files_model.h"
#include "checklistmodel.h"
#include "checklistui.h"


ChkLstEntry::ChkLstEntry(QObject* parent)
    :QObject(parent)
{
    m_Name = "";
    m_Checked = false;
    m_chkdDateTime = "";
    m_filename = "";
}


ChkLstModel::ChkLstModel(QObject *parent )
    :QAbstractListModel(parent)
{
}


QHash<int, QByteArray> ChkLstModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(NameRole, "entry_name");
    roleNames.insert(CheckedRole, "entry_checked");
    roleNames.insert(CheckedDateTimeRole, "entry_date");
    return roleNames;
}

int ChkLstModel::rowCount(const QModelIndex & parent) const {

    return m_entrys.count();
}


ChkLstEntry* ChkLstModel::getEntry(int index){
    if(index < rowCount() && index >=0){
        return m_entrys.at(index);
    }
    return NULL;
}

void ChkLstModel::refresh(ChkLstEntry* entry){
    for (int i=0; i <m_entrys.count(); i++){
        if(entry == m_entrys.at(i)){
            emit dataChanged(index(i,0), index(i,0));
            return;
        }
    }
}

void ChkLstModel::addEntry(ChkLstEntry* entry){
    beginInsertRows(QModelIndex(), m_entrys.count() ,m_entrys.count());
    m_entrys.append(entry);
    endInsertRows();
}

QVariant ChkLstModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    QVariant value;
    ChkLstEntry* entry = m_entrys.at(index.row());
    if(!entry) QVariant();

    switch(role){
    case NameRole:{
        value = entry->entryName();
        break;
    }
    case CheckedRole:{
        value = entry->checked();
        break;
    }
    case CheckedDateTimeRole:{
        value = entry->chkdDateTime();
        break;
    }
    default:
        return QVariant();
    }

    return value;
}




ChecklistPane::ChecklistPane(QObject* parent) : FilesBase(parent)
{
    m_chklstItemModel = new ChkLstItemModel(this);

    m_chklstSubItemModel = new ChkLstSubItemModel(this);

    m_FileModel = new FileModel(this);

    connect(m_chklstItemModel, SIGNAL(modified()), this, SLOT(modified()));

    connect(m_chklstSubItemModel, SIGNAL(modified()), this, SLOT(modified()));

    connect(m_chklstSubItemModel, SIGNAL(stateChanged(ChkLstItem*)), m_chklstItemModel, SLOT(itemStateChange(ChkLstItem*)));

    QRegExp filter("\\.xmchklst\\s*$");
    m_FileModel->setFilter(filter);

}

ChecklistPane::~ChecklistPane()
{
}

void ChecklistPane::setContext(QQmlContext* root)
{
    root->setContextProperty("checklistpane", this);
    root->setContextProperty("chkItemModel", m_chklstItemModel);
    root->setContextProperty("chkSubItemModel", m_chklstSubItemModel);
    m_FileModel->setModelContext("chklst_files", root);

}

void ChecklistPane::initExecute(ChkLstEntry* entry)
{

    m_entry = entry;
    QString fileName;

    if(m_entry)
        fileName = entry->fileName();

    if (!fileName.isEmpty())
        m_checklistFile.open(fileName);
    m_type = chklstExecute;
    m_checkListName = m_checklistFile.getNewKey("name").getString();
    m_description = m_checklistFile.getNewKey("description").getString();
    m_partid = m_checklistFile.getNewKey("partid").getString();
    m_weld = m_checklistFile.getNewKey("weld").getString();
    m_bead = m_checklistFile.getNewKey("bead").getString();
    m_creator = m_checklistFile.getNewKey("creator").getString();
    m_createdDateTime = m_checklistFile.getNewKey("createdDateTime").getString();
    m_revisionCount = m_checklistFile.getNewKey("revisedCount").getInt();
    m_revisedDateTime = m_checklistFile.getNewKey("revisedDateTime").getString();
    m_completed = m_checklistFile.getNewKey("completed").getBool();

    connect(m_chklstItemModel, SIGNAL(itemsComplete()),
            this, SLOT(itemsComplete()));

    SettingsIONode node = m_checklistFile.getNewKey("items");
    m_chklstItemModel->load(node);
    m_modified = false;

    emit checkListHasChanged();

}

void ChecklistPane::initEdit(const QString &fileName)
{

    m_entry = NULL;

    if (!fileName.isEmpty())
        m_checklistFile.open(fileName);
    m_type = chklstEdit;
    m_checkListName = m_checklistFile.getNewKey("name").getString();
    m_creator = m_checklistFile.getNewKey("creator").getString();
    m_description = m_checklistFile.getNewKey("description").getString();
    m_createdDateTime = m_checklistFile.getNewKey("createdDateTime").getString();
    m_revisedDateTime = m_checklistFile.getNewKey("revisedDateTime").getString();
    m_revisionCount = m_checklistFile.getNewKey("revisedCount").getInt();
    if(m_checkListName.isEmpty()){
        m_checkListName = "<untitled>";
        m_creator = ModuleConfiguration->currentUser();
        m_createdDateTime = QDateTime::currentDateTime().toString("MMM/dd/yyyy hh:mm:ss");
        m_revisedDateTime = m_createdDateTime;
        m_modified = true;
    }
    SettingsIONode node = m_checklistFile.getNewKey("items");
    m_chklstItemModel->load(node);
    m_modified = false;

    emit checkListHasChanged();

}

void ChecklistPane::modified()
{
    m_modified = true;
}

void ChecklistPane::itemSelected(int itemIndex)
{
    ChkLstItem* item = m_chklstItemModel->getItem(itemIndex);
    m_chklstSubItemModel->load(item);
}

void ChecklistPane::itemsComplete()
{
    if(m_entry && !m_entry->checked()){
        m_entry->setChecked(true);
        m_entry->setDateTime(QDateTime::currentDateTime().toString("MMM/dd/yyyy hh:mm:ss"));
    }
}


void ChecklistPane::getDirectory(QDir& dir){
    dir.setPath(Paths::chklists());
    QString path = dir.path();
    dir.setPath(path + "/chklsttmpls" );
    if(!dir.exists()){
        QDir dataDir(path);
        if(!dataDir.mkdir("chklsttmpls"))
            return;
    }
}

void ChecklistPane::loadFiles(){
    QUrl internalUrl;
    internalUrl.setScheme("file");
    QDir chkLstDir;
    getDirectory(chkLstDir);
    internalUrl.setPath(chkLstDir.path());
    load_file_list(internalUrl, m_FileModel);
}

QString ChecklistPane::getFilename(){
    QFileInfo file_info(m_checklistFile.filename);
    QString filename = file_info.baseName();
    if(filename.isEmpty())
        filename = "<untitled>";
    return filename;

}

void ChecklistPane::load_chklstFile(QString file)
{
    if(!file.isEmpty()){
        QFileInfo file_info(file);
        QString filename = file_info.baseName();
        QDir dir;
        getDirectory(dir);
        file_info.setFile(dir, filename + ".xmchklst");

        initEdit(file_info.absoluteFilePath());
    }

}

void ChecklistPane::new_chklstFile()
{
    m_checklistFile = SettingsIO();
    initEdit(QString());
}

ChkLstEntry* ChecklistPane::close()
{
    if(m_type == chklstExecute){

        if(m_entry->checked()){
            m_completed = true;
            m_revisedDateTime = m_entry->chkdDateTime();
        }else{
            m_revisedDateTime = QDateTime::currentDateTime().toString("MMM/dd/yyyy hh:mm:ss");
        }
        m_checklistFile.getNewKey("revisedDateTime").setString(m_revisedDateTime);
        m_checklistFile.getNewKey("completed").setBool(m_completed);
        SettingsIONode node = m_checklistFile.getNewKey("items");
        m_chklstItemModel->save(node);
        m_checklistFile.save();
    }
    return(m_entry);
}

QString ChecklistPane::saveAs()
{
    loadFiles();
    QFileInfo file_info(m_checklistFile.filename);
    QString filename = file_info.baseName();
    return filename;
}

void ChecklistPane::save_chklstFile(QString file, bool exit)
{

    if(m_type == chklstEdit){
        emit saving();
        QFileInfo file_info(file);
        QString filename = file_info.baseName();
        m_checkListName = filename;
        m_checklistFile.getNewKey("name").setString(m_checkListName);
        m_checklistFile.getNewKey("creator").setString(m_creator);
        m_checklistFile.getNewKey("description").setString(m_description);
        m_checklistFile.getNewKey("createdDateTime").setString(m_createdDateTime);
        m_revisedDateTime = QDateTime::currentDateTime().toString("MMM/dd/yyyy hh:mm:ss");
        m_checklistFile.getNewKey("revisedDateTime").setString(m_revisedDateTime);
        m_revisionCount += 1;
        m_checklistFile.getNewKey("revisedCount").setInt(m_revisionCount);
        QDir dir;
        getDirectory(dir);
        file_info.setFile(dir, filename + ".xmchklst");
        SettingsIOFragment node;
        m_chklstItemModel->save(node);
        SettingsIONode items_node = m_checklistFile.getNewKey("items");
        items_node.replace( node);
        m_modified = false;
        m_checklistFile.save(file_info.absoluteFilePath());

    }
    emit checkListHasChanged();
    if(exit)
        emit chklstExit();

}

void ChecklistPane::saveAndExit()
{
    QString filename = m_checklistFile.filename;
    save_chklstFile(filename, true);
}


ChecklistUI::ChecklistUI(QObject* parent)
    :FilesBase(parent)
{

    QDir chkLstDir;
    getDirectory(chkLstDir);

    m_chklstModel = new ChkLstModel(this);

    chkLstDir.setSorting(QDir::Time);
    QStringList filters;
    filters << "*.xmchklst";
    chkLstDir.setNameFilters(filters);
    QFileInfoList entryList = chkLstDir.entryInfoList();
    for(int i =0; i< entryList.count(); i++){
        QFileInfo fileinfo = entryList.at(i);
        SettingsIO file(fileinfo.absoluteFilePath());
        ChkLstEntry* entry = new ChkLstEntry(m_chklstModel);
        entry->setFilename(file.filename);
        entry->setName(file.getKey("name").getString());
        entry->setChecked(file.getKey("completed").getBool());
        entry->setDateTime(file.getKey("revisedDateTime").getString());
        m_chklstModel->addEntry(entry);
    }


    m_FileModel = new FileModel(this);

    QRegExp filter("\\.xmchklst\\s*$");
    m_FileModel->setFilter(filter);

}

ChecklistUI::~ChecklistUI()
{
}

void ChecklistUI::setContext(QQmlContext* root)
{
    m_cntxt = root;
    root->setContextProperty("checklistui", this);
    root->setContextProperty("chklists", m_chklstModel);
    m_FileModel->setModelContext("tmpl_files", root);

}

void ChecklistUI::getDirectory(QDir& dir){
    dir.setPath(Paths::chklists());
    QString path = dir.path();
    dir.setPath(path + "/" + xmui->part_id());
    if(!dir.exists()){
        QDir dataDir(path);
        if(!dataDir.mkdir(xmui->part_id()))
            return;
    }
}

void ChecklistUI::getTmplDirectory(QDir& dir){
    dir.setPath(Paths::chklists());
    QString path = dir.path();
    dir.setPath(path + "/chklsttmpls" );
    if(!dir.exists()){
        QDir dataDir(Paths::chklists());
        if(!dataDir.mkdir("chklsttmpls"))
            return;
    }
}

bool ChecklistUI::getTemplate(QString name, SettingsIONode& node){
    QFileInfo fileinfo(name);
    QString filename = fileinfo.baseName();

    QDir dir;
    getTmplDirectory(dir);
    QFileInfo file(dir, filename + ".xmchklst");
    SettingsIO tmpl(file.absoluteFilePath());
    node.replace(tmpl);
    node.getNewKey("name").setString(fileinfo.baseName());
    node.getNewKey("partid").setString(xmui->part_id());
    node.getNewKey("weld").setString(xmui->weldNum());
    node.getNewKey("bead").setString(xmui->bead());
    node.getNewKey("creator").setString(ModuleConfiguration->currentUser());
    node.getNewKey("createdDateTime").setString(QDateTime::currentDateTime().toString("MMM/dd/yyyy hh:mm:ss"));
    node.getNewKey("revisedDateTime").setString(QDateTime::currentDateTime().toString("MMM/dd/yyyy hh:mm:ss"));
    node.getNewKey("completed").setBool(false);
    return true;
}


void ChecklistUI::editChecklist(int index){
    if((index < 0) && (index < m_chklstModel->rowCount())){
        m_ChecklistPane = new ChecklistPane(this);
        m_ChecklistPane->setContext(m_cntxt);
        m_ChecklistPane->initEdit(QString());
    }else{
        ChkLstEntry* entry =  m_chklstModel->getEntry(index);
        if(entry){
            m_ChecklistPane = new ChecklistPane(this);
            m_ChecklistPane->setContext(m_cntxt);

            QDir dir;
            getTmplDirectory(dir);
            QFileInfo fileinfo(entry->entryName());
            QString filename = fileinfo.baseName() + ".xmchklst";
            QFileInfo file(dir,filename);

            m_ChecklistPane->initEdit(file.absoluteFilePath());
        }
    }
}

void ChecklistUI::closeChecklist(){
    if(m_ChecklistPane){
        ChkLstEntry* entry = m_ChecklistPane->close();
        if(m_ChecklistPane->type() == ChecklistPane::chklstExecute){
            m_chklstModel->refresh(entry);
        }
        m_ChecklistPane->deleteLater();
    }
    m_ChecklistPane = NULL;
}

void ChecklistUI::runChecklist(int index){

    if((index < 0) && (index < m_chklstModel->rowCount())){
        return;
    }
    ChkLstEntry* entry =  m_chklstModel->getEntry(index);
    if(entry){
        m_ChecklistPane = new ChecklistPane(this);
        m_ChecklistPane->setContext(m_cntxt);
        m_ChecklistPane->initExecute(entry);
    }

}

void ChecklistUI::addEntry(QString name){
    QFileInfo fileinfo(name);
    QDir dir;
    getDirectory(dir);
    QString filename = fileinfo.baseName() +"_"+ QDateTime::currentDateTime().toString("MMM-dd-yyyy_hh:mm:ss") + ".xmchklst";
    QFileInfo file(dir,filename);
    SettingsIO node;
    if(getTemplate(name, node)){
        node.save(file.absoluteFilePath());
        ChkLstEntry* entry = new ChkLstEntry(m_chklstModel);
        entry->setFilename(node.filename);
        entry->setName(node.getKey("name").getString());
        entry->setChecked(false);
        entry->setDateTime(QDateTime::currentDateTime().toString("MMM-dd-yyyy hh:mm:ss"));
        m_chklstModel->addEntry(entry);
    }
}


void ChecklistUI::loadFiles(){
    QUrl internalUrl;
    internalUrl.setScheme("file");
    QDir chkLstDir;
    getTmplDirectory(chkLstDir);
    internalUrl.setPath(chkLstDir.path());
    load_file_list(internalUrl, m_FileModel);
}

//#include "checklistui.moc"
