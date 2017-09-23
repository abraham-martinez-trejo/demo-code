#ifndef CHECKLISTUI_H
#define CHECKLISTUI_H


#include "settingsio.h"
#include "files_base.h"
#include <QAbstractListModel>

class ChkLstSubItemModel;
class ChkLstItemModel;
class QQmlContext;
class QDir;

class ChkLstEntry: public QObject
{
public:
    ChkLstEntry();
    ChkLstEntry(QObject *parent = 0);
    QString fileName() {return m_filename;}
    void setFilename(QString str) {m_filename = str;}
    QString entryName() {return m_Name;}
    void setName(QString str) {m_Name = str;}
    bool checked() {return m_Checked;}
    void setChecked(bool checked) {m_Checked = checked;}
    QString chkdDateTime() {return m_chkdDateTime;}
    void setDateTime(QString str) {m_chkdDateTime = str;}

protected:
    bool m_Checked;
    QString m_filename;
    QString m_Name;
    QString m_chkdDateTime;

};

class ChkLstModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ItemRoles {
        NameRole = Qt::UserRole + 1,
        CheckedRole,
        CheckedDateTimeRole
    };

    ChkLstModel(QObject *parent = 0);
    void setContext(QQmlContext* root);
    ChkLstEntry* getEntry(int index);
    void addEntry(ChkLstEntry* entry);
    void refresh(ChkLstEntry* entry);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList <ChkLstEntry*> m_entrys;
};

class ChecklistPane : public FilesBase
{
    Q_OBJECT

    Q_PROPERTY(QString checkList_name READ checkList_name WRITE set_checkList_name NOTIFY checkListHasChanged)
    Q_PROPERTY(QString description READ description WRITE set_description NOTIFY checkListHasChanged)
    Q_PROPERTY(QString partid READ partid WRITE set_partid NOTIFY checkListHasChanged)
    Q_PROPERTY(QString weld READ weld WRITE set_weld NOTIFY checkListHasChanged)
    Q_PROPERTY(QString bead READ bead WRITE set_bead NOTIFY checkListHasChanged)
    Q_PROPERTY(QString creator READ creator WRITE set_creator NOTIFY checkListHasChanged)
    Q_PROPERTY(QString createdDateTime READ createdDateTime WRITE set_createdDateTime NOTIFY checkListHasChanged)
    Q_PROPERTY(QString revisedDateTime READ revisedDateTime WRITE set_revisedDateTime NOTIFY checkListHasChanged)
    Q_PROPERTY(int revisionCount READ revisionCount NOTIFY checkListHasChanged)
    Q_PROPERTY(bool completed READ completed WRITE set_completed NOTIFY checkListHasChanged)


public:
    enum chklstType{
        chklstExecute = 0,
        chklstEdit
    };

    ChecklistPane(QObject *parent = 0);
    ~ChecklistPane();

    int  type() {return m_type;}
    void setContext(QQmlContext* root);
    void initExecute(ChkLstEntry* _entry);
    void initEdit(const QString &file_name);

    QString checkList_name() {return m_checkListName;}
    QString description() {return m_description;}
    QString partid() {return m_partid;}
    QString weld() {return m_weld;}
    QString bead() {return m_bead;}
    QString creator() {return m_creator;}
    QString createdDateTime() {return m_createdDateTime;}
    QString revisedDateTime() {return m_revisedDateTime;}
    int revisionCount() {return m_revisionCount;}
    bool completed() {return m_completed;}

    void set_checkList_name(QString str) {m_checkListName = str;}
    void set_description(QString str) {m_description = str;}
    void set_partid(QString str) {m_partid = str;}
    void set_weld(QString str) {m_weld = str;}
    void set_bead(QString str) {m_bead = str;}
    void set_creator(QString str) {m_creator = str;}
    void set_createdDateTime(QString str) {m_createdDateTime = str;}
    void set_revisedDateTime(QString str) {m_revisedDateTime = str;}
    void set_completed(bool value) {m_completed = value;}

    void getDirectory(QDir& dir);
    ChkLstEntry* close();

signals:
    void checkListHasChanged();
    void chklstExit();
    void saving();


public slots:
    bool isModified() { return m_modified;}
    void itemSelected(int itemIndex);
    void modified();
    void loadFiles();
    QString getFilename();
    void load_chklstFile(QString file);
    void new_chklstFile();
    QString saveAs();
    void save_chklstFile(QString name, bool exit = false);
    void saveAndExit();
    void itemsComplete();

protected:
    SettingsIO m_checklistFile;

    FileModel* m_FileModel;
    ChkLstEntry* m_entry;
    ChkLstItemModel* m_chklstItemModel;
    ChkLstSubItemModel* m_chklstSubItemModel;

    bool m_modified;
    int m_type;

    QString m_checkListName;
    QString m_description;
    QString m_partid;
    QString m_weld;
    QString m_bead;
    QString m_creator;
    QString m_createdDateTime;
    QString m_revisedDateTime;
    int m_revisionCount;
    bool m_completed;
};

class ChecklistUI : public FilesBase
{
    Q_OBJECT

public:
    ChecklistUI(QObject *parent = 0);
    ~ChecklistUI();

    void setContext(QQmlContext* root);
    void init(int type, const QString &file_name);
    void getDirectory(QDir& dir);
    void getTmplDirectory(QDir& dir);
    bool getTemplate(QString name, SettingsIONode& node);
signals:


public slots:
    void loadFiles();
    void editChecklist(int index);
    void runChecklist(int index);
    void closeChecklist();
    void addEntry(QString name);
protected:
    QQmlContext* m_cntxt;
    FileModel* m_FileModel;
    ChkLstModel* m_chklstModel;
    ChecklistPane* m_ChecklistPane;
};

#endif // CHECKLISTUI_H
