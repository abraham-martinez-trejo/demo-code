#include "usersmodel.h"


PermissionsModel::PermissionsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    int all_permissions = 0xffe7;
    // use hex 0xffe7 to exclude edit password etc
    m_PermissionList = permissions_to_qstringlist((ModuleConfig_Class::Permissions)all_permissions);
    m_UserPermissions.clear();
}

QHash<int, QByteArray> PermissionsModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name_";
    roles[StateRole] = "state_";
    return roles;
}

void PermissionsModel::setContext(QQmlContext *root)
{
    root->setContextProperty("permissionsModel", this);
}

void PermissionsModel::setPermissions(int permissions)
{
    m_UserPermissions = permissions_to_qstringlist((ModuleConfig_Class::Permissions)permissions);
    emit dataChanged(index(0,0),index(m_PermissionList.count()-1, 0));
}

int PermissionsModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_PermissionList.count();
}

QVariant PermissionsModel::data(const QModelIndex &index, int role) const {
    if (index.row() <  0 || index.row() >= m_PermissionList.count())
        return QVariant();

    if(role == NameRole)
        return m_PermissionList.at(index.row());
    else if(role == StateRole)
        return m_UserPermissions.contains( m_PermissionList.at(index.row()));
    return QVariant();
}

bool PermissionsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch(role){
    case StateRole:
        if(value.toBool()){
            m_UserPermissions.append(m_PermissionList.at(index.row()));
        }else{
            m_UserPermissions.removeAll(m_PermissionList.at(index.row()));
        }
        emit dataChanged(index, index);
        break;
    }

    return true;

}




UsersModel::UsersModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_UserNodes = Settings->settings.getNewKey("users");
}

QHash<int, QByteArray> UsersModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IDRole] = "id";
    roles[NameRole] = "name";
    roles[GroupRole] = "group";
    roles[PasswordRole] = "password";
    roles[PermissionsRole] = "permissions";
    return roles;
}

void UsersModel::setContext(QQmlContext *root)
{
    root->setContextProperty("usersModel", this);
}

void UsersModel::reset()
{
    beginResetModel();
    m_users.clear();
    endResetModel();
}

int UsersModel::add(const QString &userID)
{
    beginInsertRows(QModelIndex(), m_users.count(), m_users.count());
    m_users << userID;
    m_UserNodes.getNewKey(userID);
    endInsertRows();
    return m_users.count()-1;
}

bool UsersModel::hasUser(const QString& user)
{
    int index = findUserIndex(user);
    if(index < 0){
        return false;
    }
    return true;
}

QString UsersModel::getUserName(const QString& user)
{
    int index = findUserIndex(user);
    if(index < 0){
        return QString();
    }
    return getName(index);
}


QString UsersModel::getUserPassword(const QString& user)
{
    int index = findUserIndex(user);
    if(index < 0){
        return QString();
    }
    return getPassword(index);
}

void UsersModel::getUserPermissions(const QString& user,
                                    ModuleConfig_Class::Permissions &permissions)
{
    int index = findUserIndex(user);
    if(index < 0){
        permissions = ModuleConfig_Class::NoPermission;
        return ;
    }
    permissions = (ModuleConfig_Class::Permissions)getPermissions(index).toInt();
}

void UsersModel::getUserPermissions(const QString& user, QStringList& permissionsList)
{

    int index = findUserIndex(user);
    if(index < 0){
        permissionsList.clear();
        return ;
    }
    ModuleConfig_Class::Permissions permissions =
            (ModuleConfig_Class::Permissions)getPermissions(index).toInt();
    permissionsList = permissions_to_qstringlist( permissions);
}

int UsersModel::findUserIndex(const QString& _user)
{
    for(int i=0; i< m_users.count(); i++){
        QString user = m_users.at(i);
        if(_user == user)
            return i;
    }
    return -1;
}

QString UsersModel::getUser(const int rowIndex)
{
    return m_users.at(rowIndex);
}

void UsersModel::setName(int _index, const QString& name)
{
    setData(index(_index, 0), QVariant(name), NameRole);
}

QString UsersModel::getName(const int rowIndex)
{
    return data(index(rowIndex,0), NameRole).toString();
}

void UsersModel::setGroup(int _index, const QString& group)
{
    setData(index(_index, 0), QVariant(group), GroupRole);
}

QString UsersModel::getGroup(const int rowIndex){

    return data(index(rowIndex,0), GroupRole).toString();
}

void UsersModel::setPassword(int _index, const QString& password)
{
    setData(index(_index, 0), QVariant(password), PasswordRole);
}

QString UsersModel::getPassword(const int rowIndex)
{
    return data(index(rowIndex,0), PasswordRole).toString();
}

void UsersModel::setPermissions(int _index, const QString& permissions)
{
    setData(index(_index, 0), QVariant(permissions), PermissionsRole);
}

QString UsersModel::getPermissions(const int rowIndex)
{
    return data(index(rowIndex,0), PermissionsRole).toString();
}

void UsersModel::removeUser(const int &rowIndex)
{
    beginRemoveRows(QModelIndex(), rowIndex, rowIndex); // 0, m_members.count() - 1);
    QString userID = m_users.takeAt(rowIndex);
    m_UserNodes.deleteKey(userID);
    endRemoveRows();
}










void UsersModel::sortUserIDAlphabetically()
{
    for (int i = 0; i < m_users.size(); i++) {
        for (int j = 0; j < m_users.size(); j++) {

            QString iString = m_users[i];
            QString jString = m_users[j];
            iString = iString.toUpper();
            jString = jString.toUpper();

            if (jString > iString)
                m_users.move(i,j);
        }
    }
    emit layoutChanged();
}


void UsersModel::sortUserNameAlphabetically()
{
    for (int i = 0; i < m_users.size(); i++) {
        for (int j = 0; j < m_users.size(); j++) {

//            QString iString = m_users[i];
//            QString jString = m_users[j];

            QString iString = m_users[i];
            QString jString = m_users[j];

            iString = iString.toUpper();
            jString = jString.toUpper();

            if (jString > iString)
                m_users.move(i,j);
        }
    }
    emit layoutChanged();
}







int UsersModel::rowCount(const QModelIndex &parent) const {
    return m_users.count();
}

QVariant UsersModel::data(const QModelIndex &index, int role) const {
    if (index.row() <  0 || index.row() >= m_users.count())
        return QVariant();


    QString userID = m_users.at(index.row());
    SettingsIONode user_node = m_UserNodes.getKey(userID);
    if(!user_node.isNull()){
        if (role == IDRole)
            return user_node.key();
        else if(role == NameRole)
            return user_node.getNewKey("name").getString();
        else if(role == GroupRole)
            return user_node.getNewKey("group").getString();
        else if(role == PasswordRole)
            return user_node.getNewKey("password").getString();
        else if(role == PermissionsRole)
            return user_node.getNewKey("permissions").getString();
    }
    return QVariant();
}

bool UsersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

    QString userID = m_users.at(index.row());
    SettingsIONode idNode = m_UserNodes.getKey(userID);
    if(idNode.isNull())
        return false;

    switch(role){
    case NameRole:
        idNode.getNewKey("name").setString(value.toString());
        break;
    case GroupRole:
        idNode.getNewKey("group").setString(value.toString());
        break;
    case PasswordRole:
        idNode.getNewKey("password").setString(value.toString());
        break;
    case PermissionsRole:
        idNode.getNewKey("permissions").setString(value.toString());
        break;
    }
    emit dataChanged(index, index);

    return true;

}

