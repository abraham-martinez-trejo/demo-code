#ifndef SETTINGSDBMS_H
#define SETTINGSDBMS_H

// Qt classes
#include <QDebug>
#include <QString>
#include <QStringList>

#include <QQmlContext>

// Sean Marble's classes
#include "modconfig.h"

// unsorted classes
#include "setup_ui.h"
#include "settingsio.h"
#include "websocketclient.hpp"

class SettingsIONode;

/*  SettingsDBMS
 *
 *  The SettingsDBMS class contains members associated
 *  with the users database from the settings.xms file.
 *  The methods are utilized for manipulating data, and
 *  member variables are utilized for storing created
 *  or modified data in this class.
 *
 *  The settings.xms file can be found on the sequencer.
 *
 */

class SettingsDBMS
{
public:
    // constructor
    SettingsDBMS();

    // validation methods
    bool userExists(const QString &user);
    bool validPassword(const QString &user, const QString &password);

    // insert methods
    void insertUser(
            const QString &id,
            const QString &name,
            const QString &group,
            const QString &password,
            const QString &permissions
            );

    // select methods
    QString selectName(const QString &id);
    QString selectGroup(const QString &id);
    QStringList selectColumnNames();

    // all user ids
    QStringList selectAllIDs();
    // all user names
    QStringList selectAllNames();

    QStringList selectAllGroups();
    QStringList selectAllPasswords();
    QStringList selectAllPermissions();

    // update methods

    ///////////////////////////////////////////////////////////////////
    // the method below updates USER ID
    // I want to be able to update the USER NAME AND NOT USER ID
    // USER ID SHOULD ONLY BE ABLE TO BE ADD AND DELETED BUT NOT MODIFIED
    // NOR RECREATED !!!!!!!!!!!!!!!!!!!!!!!!!!
    void updateID(const QString &user, const QString &updatingUser);
    ///////////////////////////////////////////////////////////////////

    void updateName(const QString &userID, const QString &newName);
    void updateGroup(const QString &user, const QString &updatingGroup);
    void updatePassword(const QString &user, const QString &updatingPassword);
    void updatePermissions(const QString &user, const int permissions);

    // delete methods
    void deleteUser(const QString &user);

    // boolean method to check if database exists
    Q_INVOKABLE bool updateDatabase();

private:
    // users database nodes allocation
    SettingsIONode m_usersNode;
    SettingsIONode m_idNode;
    SettingsIONode m_nameNode;
    SettingsIONode m_groupNode;
    SettingsIONode m_passwordNode;
    SettingsIONode m_permissionsNode;

    /////////////////////////////////////////////////////
    // PROVIDE CONSTANTS FOR:
    // users, rootID, name, group, password, AND permissions
    /////////////////////////////////////////////////////


};

#endif // SETTINGSDBMS_H
