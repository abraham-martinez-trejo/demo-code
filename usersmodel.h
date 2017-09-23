#ifndef USERSMODEL_H
#define USERSMODEL_H

// Qt libraries
#include <QAbstractListModel>
#include <QStringList>
#include <QQmlContext>
#include <QQuickWidget>
#include <QObject>

#include "../controller/settings.h"
#include "../controller/settingsio.h"
#include "../controller/modconfig.h"

class PermissionsModel : public QAbstractListModel
{
    Q_OBJECT
public:

    enum PermissionRoles {
        NameRole,
        StateRole
    };

    PermissionsModel(QObject *parent = 0);

    // manually setting context through a method call
    void setContext(QQmlContext *root);

    bool setData(const QModelIndex &index, const QVariant &value, int role);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

protected:
    QHash<int, QByteArray> roleNames() const;

public slots:
    void setPermissions(int permissions);

public:
    QStringList m_UserPermissions;
    QStringList m_PermissionList;

};

class UsersModel : public QAbstractListModel
{
    Q_OBJECT
public:

    // manually setting context through a method call
    void setContext(QQmlContext *root);

    enum UserRoles {
        IDRole = Qt::UserRole + 1,
        NameRole,
        GroupRole,
        PasswordRole,
        PermissionsRole
    };

    UsersModel(QObject *parent = 0);

    void reset();

    int add(const QString &userID);

    int findUserIndex(const QString& _user);

    void removeUser(const int &rowIndex);

    // Get specific cell contents.
    void setName(int _index, const QString& name);
    void setGroup(int _index, const QString& group);
    void setPassword(int _index, const QString& password);
    void setPermissions(int _index, const QString& permissions);

    bool hasUser(const QString& user);
    QString getUserName(const QString& user);
    QString getUserPassword(const QString& user);
    void getUserPermissions(const QString& user,
                            ModuleConfig_Class::Permissions &permissions);
    void getUserPermissions(const QString& user, QStringList& permissionsList);

    Q_INVOKABLE void sortUserIDAlphabetically();
    Q_INVOKABLE void sortUserNameAlphabetically();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    // editable methods
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:

    QString getUser(const int rowIndex);
    QString getName(const int rowIndex);
    QString getGroup(const int rowIndex);
    QString getPassword(const int rowIndex);
    QString getPermissions(const int rowIndex);

protected:
    SettingsIONode m_UserNodes;
    QHash<int, QByteArray> roleNames() const;

public:
    QStringList m_users;

};

#endif // USERSMODEL_H
