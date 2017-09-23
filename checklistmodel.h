
#ifndef CHECKLISTMODEL_H
#define CHECKLISTMODEL_H

#include "../controller/settings.h"
#include <QObject>
#include <QStringList>
#include <QtQml>

class QQmlContext;
class FileEntry;
class FileModel;

class ChkLstSubItem: public QObject
{
public:
    enum chkdStatus{
        chkdNone = 0,
        chkdOperator = 0x1,
        chkdEngineer = 0x2,
        chkdComplete = 0x3
    };

    ChkLstSubItem();
    ChkLstSubItem(SettingsIONode &node);
    void save(SettingsIONode& node);
    bool setText(QString str);
    QString itemText() {return m_Text;}
    bool setOperator(QString str);
    bool setEngineer(QString str);
    QString operatorId() {return m_OperatorId;}
    QString engineerId() {return m_EngineerId;}
    int checkState() {return m_CheckedState;}
    QString chkdDateTime() {return m_chkdDateTime;}

protected:
    int m_CheckedState;
    QString m_Text;
    QString m_OperatorId;
    QString m_EngineerId;
    QString m_chkdDateTime;
};

class ChkLstItem: public QObject
{
    Q_OBJECT

public:
    ChkLstItem();
    ChkLstItem(SettingsIONode &node);
    ~ChkLstItem();
    void save(SettingsIONode& node);
    ChkLstSubItem* getItem(int index);
    bool setText(QString str);
    QString itemText() {return m_Text;}
    bool checked() {return m_Checked;}
    QString chkdDateTime() {return m_chkdDateTime;}
    int itemCount() { return m_subItems.count();}
    void append(ChkLstSubItem* item);
    void insert(int index, ChkLstSubItem* item);
    void remove(int index);
    void move(int fromIndex, int toIndex);

public slots:
    void itemChecked(int state);

protected:
    bool m_Checked;
    bool m_itemsInProcess;
    int m_itemsChecked;
    QString m_Text;
    QString m_chkdDateTime;
    QList <ChkLstSubItem*> m_subItems;

};

class ChkLstSubItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum SubItemRoles {
        ItemIdRole = Qt::UserRole + 1,
        TextRole,
        OperatorRole,
        EngineerRole,
        CheckStateRole,
        CheckedDateTimeRole
    };

    ChkLstSubItemModel(QObject *parent = 0);

    void load(ChkLstItem* chklistItem);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

public slots:
    void addItem(int rowIndex);
    void removeItem(int rowIndex);
    void moveItem(int fromIndex, int toIndex);
    void operatorCheck(int index, QString user);
    void engineerCheck(int index, QString user);

signals:
    void stateChanged(ChkLstItem*);
    void modified();

protected:
    QString m_itemId;

    ChkLstItem* m_chkLstItem;
    QHash<int, QByteArray> roleNames() const;

private:

    friend class ChkLstItem;
};

class ChkLstItemModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ItemRoles {
        TextRole = Qt::UserRole + 1,
        CheckedRole,
        CheckedDateTimeRole
    };

    ChkLstItemModel(QObject *parent = 0);
    void load(SettingsIONode &node);
    void save(SettingsIONode& node);
    int subItemCount();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

signals:
    void modified();
    void itemsComplete();

public slots:
    void reset();
    ChkLstItem* getItem(int index);
    void addItem(int rowIndex);
    void removeItem(int rowIndex);
    void moveItem(int fromIndex, int toIndex);
    void itemStateChange(ChkLstItem*);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList <ChkLstItem*> m_items;
};

#endif // CHECKLISTMODEL_H
