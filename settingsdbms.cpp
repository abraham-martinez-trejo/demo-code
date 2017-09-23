#include "settingsdbms.h"

SettingsDBMS::SettingsDBMS()
{
}

bool SettingsDBMS::updateDatabase()
{
    if (Settings->settings.hasKey("users")){
        // do something
    } else {
        m_usersNode = Settings->settings.newKey("users");
        m_idNode = m_usersNode.newKey("rootID");
        m_nameNode = m_idNode.newKey("name");
        m_nameNode.setString("rootName");
        m_groupNode = m_idNode.newKey("group");
        m_groupNode.setString("Super User");
        m_permissionsNode = m_idNode.newKey("permissions");
        m_permissionsNode.setString("7");

        m_passwordNode = m_idNode.newKey("password");
        m_passwordNode.setString("password");

        WebSocketClient::settings_merge(m_usersNode);
        WebSocketClient::settings_file_save();
    }
}

QStringList SettingsDBMS::selectColumnNames()
{
    //    QStringList someString gList;
    //    someStringList << Settings->settings.getKey();
}

QStringList SettingsDBMS::selectAllIDs()
{
    // Local variable declaration and intitialization.
    QStringList idList;

    m_usersNode = Settings->settings.getKey("users");
    for (SettingsIONode::iterator i = m_usersNode.begin(); i != m_usersNode.end(); i++) {
        if (m_usersNode.hasKey(i.key())) {
            idList << i.key();
        }
    }

    return idList;
}

QStringList SettingsDBMS::selectAllNames()
{
    // Local variable declaration and initialization
    QStringList nameList;

    m_usersNode = Settings->settings.getKey("users");
    for (SettingsIONode::iterator i = m_usersNode.begin(); i != m_usersNode.end(); i++) {
        if (m_usersNode.hasKey(i.key())) {
            m_idNode = m_usersNode.getKey(i.key());
            m_groupNode = m_idNode.getKey("name");
            nameList << m_groupNode.getString();
        }
    }

    return nameList;
}

QStringList SettingsDBMS::selectAllGroups()
{
    // Local variable declaration and intitialization.
    QStringList groupList;

    m_usersNode = Settings->settings.getKey("users");
    for (SettingsIONode::iterator i = m_usersNode.begin(); i != m_usersNode.end(); i++) {
        if (m_usersNode.hasKey(i.key())) {
            m_idNode = m_usersNode.getKey(i.key());
            m_groupNode = m_idNode.getKey("group");
            groupList << m_groupNode.getString();
        }
    }

    return groupList;
}

QStringList SettingsDBMS::selectAllPasswords()
{
    // Local variable declaration and initialization.
    QStringList passwordList;

    m_usersNode = Settings->settings.getKey("users");
    for (SettingsIONode::iterator i = m_usersNode.begin(); i != m_usersNode.end(); i++) {
        if (m_usersNode.hasKey(i.key())) {
            m_idNode = m_usersNode.getKey(i.key());
            m_passwordNode = m_idNode.getKey("password");
            passwordList << m_passwordNode.getString();
        }
    }

    return passwordList;
}

QStringList SettingsDBMS::selectAllPermissions()
{
    // Local variable declaration and initialization.
    QStringList permissionList;

    m_usersNode = Settings->settings.getKey("users");
    for (SettingsIONode::iterator i = m_usersNode.begin(); i != m_usersNode.end(); i++) {
        if (m_usersNode.hasKey(i.key())) {
            m_idNode = m_usersNode.getKey(i.key());
            m_permissionsNode = m_idNode.getKey("permissions");
            permissionList << m_permissionsNode.getString();
        }
    }

    return permissionList;
}

bool SettingsDBMS::userExists(const QString &user)
{
    bool valid = false;

    m_usersNode = Settings->settings.getKey("users");
    if (m_usersNode.hasKey(user)) {
        valid = true;
    }

    return valid;
}

QString SettingsDBMS::selectName(const QString &id)
{
    m_usersNode = Settings->settings.getKey("users");
    if (m_usersNode.hasKey(id))
        m_idNode = m_usersNode.getKey(id);
    m_nameNode = m_idNode.getKey("name");
    return m_nameNode.getString();
}

QString SettingsDBMS::selectGroup(const QString &id)
{
    m_usersNode = Settings->settings.getKey("users");
    if (m_usersNode.hasKey(id))
        m_idNode = m_usersNode.getKey(id);
    m_groupNode = m_idNode.getKey("group");
    return m_groupNode.getString();
}

bool SettingsDBMS::validPassword(const QString &user, const QString &password)
{
    // initializing local variables
    QString permissionString;
    bool valid = false;

    // Traverse and fetch data in the settings.xms file
    m_usersNode = Settings->settings.getKey("users");
    if (m_usersNode.hasKey(user)) {
        m_idNode = m_usersNode.getKey(user);
        m_passwordNode = m_idNode.getKey("password");
        m_permissionsNode = m_idNode.getKey("permissions");
        permissionString = m_permissionsNode.getString();

        // Compare settings.xms password to argument passed password
        if (m_passwordNode.getString() == password) {
            ModuleConfiguration->setUser(user, QFlag(permissionString.toInt()) );
            valid = true;
        }
    }

    return valid;
}

void SettingsDBMS::insertUser(
        const QString &id,
        const QString &name,
        const QString &group,
        const QString &password,
        const QString &permissions)
{
    // Traverse and fetch data through settings.xms file.
    //m_usersNode = Settings->settings.getKey("users");
    m_idNode = m_usersNode.newKey(id);
    m_nameNode = m_idNode.newKey("name");
    m_nameNode.setString(name);
    m_groupNode = m_idNode.newKey("group");
    m_groupNode.setString(group);
    m_passwordNode = m_idNode.newKey("password");
    m_passwordNode.setString(password);
    m_permissionsNode = m_idNode.newKey("permissions");
    m_permissionsNode.setString(permissions);

    // Make changes and save to the settings.xms file.
    WebSocketClient::settings_merge(m_usersNode);
    WebSocketClient::settings_file_save();
}

void SettingsDBMS::deleteUser(const QString &user)
{
    if (user == "rootID") {
        // HARDCODE ROOTID DELETION PREVENTION
    }
    else {
        // Traverse and delete data through settings.xms file.
        //m_usersNode = Settings->settings.getKey("users");
        m_usersNode.deleteKey(user);
        // Make changes and save to the settings.xms file.
        WebSocketClient::settings_replace(m_usersNode);
        WebSocketClient::settings_file_save();
    }
}

void SettingsDBMS::updateID(const QString &user, const QString &updatingUser)
{
    m_usersNode.renameKey(user, updatingUser);

    WebSocketClient::settings_replace(m_usersNode, m_usersNode.path());
    WebSocketClient::settings_file_save();
}

void SettingsDBMS::updateName(const QString &userID, const QString &newName) {

    m_idNode = m_usersNode.getKey(userID);
    m_nameNode = m_idNode.getKey("name");
    m_nameNode.setString(newName);

    WebSocketClient::settings_replace(m_nameNode, m_nameNode.path());
    WebSocketClient::settings_file_save();
}

void SettingsDBMS::updateGroup(const QString &user, const QString &updatingGroup)
{
    // Traverse and update data through settings.xms file.
    //m_usersNode = Settings->settings.getKey("users");
    m_idNode = m_usersNode.getKey(user);
    m_groupNode = m_idNode.getKey("group");
    m_groupNode.setString(updatingGroup);

    // Make changes and save to the settings.xms file.
    WebSocketClient::settings_replace(m_groupNode, m_groupNode.path());
    WebSocketClient::settings_file_save();
}

void SettingsDBMS::updatePermissions(const QString &user, const int permissions)
{
    // Local variable declaration and intitialization.
    QString permissionString;
    permissionString.clear();

    // Traverse users data in the settings.xms file
    //m_usersNode = Settings->settings.getKey("users");
    m_idNode = m_usersNode.getKey(user);
    m_permissionsNode = m_idNode.getKey("permissions");
    m_permissionsNode.setString(QString::number(permissions));
    permissionString = m_permissionsNode.getString();

    // heximal to integer conversion for current permission parameters
    // 0x0 == 0
    // 0x1 == 1
    // 0x2 == 2
    // 0x4 == 4
    // 0x8 == 8
    // 0x10 == 16

    // Make changes and save to the settings.xms file.
    ModuleConfiguration->setUser(m_idNode.key(), QFlag(permissionString.toInt()));
    WebSocketClient::settings_file_save();
}

void SettingsDBMS::updatePassword(const QString &user, const QString &updatingPassword)
{
    // Traverse and update data through settings.xms file.
    //m_usersNode = Settings->settings.getKey("users");
    m_idNode = m_usersNode.getKey(user);
    m_passwordNode = m_idNode.getKey("password");
    m_passwordNode.setString(updatingPassword);

    // Make changes and save to the settings.xms file.
    WebSocketClient::settings_replace(m_passwordNode, m_passwordNode.path());
    WebSocketClient::settings_file_save();
}
