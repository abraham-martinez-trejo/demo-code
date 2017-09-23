
#include <QQmlContext>

#include "../controller/settings.h"
#include "../controller/modconfig.h"
#include "../controller/weldprogram.h"
#include "../controller/websocketclient.hpp"

#include "checklistmodel.h"


/*********************************************************************/
ChkLstSubItem::ChkLstSubItem()
{
    m_Text =  "";
    m_OperatorId =  "";
    m_EngineerId = "";
    m_CheckedState =  0;
    m_chkdDateTime =  "";
}

ChkLstSubItem::ChkLstSubItem(SettingsIONode &node)
{
    m_Text =  node.getNewKey("text").getString();
    m_OperatorId =  node.getNewKey("operator").getString();
    m_EngineerId =  node.getNewKey("engineer").getString();
    m_CheckedState =  node.getNewKey("state").getInt();
    m_chkdDateTime =  node.getNewKey("checkedDateTime").getString();
}


void ChkLstSubItem::save(SettingsIONode &node)
{
    node.getNewKey("text").setString(m_Text);
    node.getNewKey("operator").setString(m_OperatorId);
    node.getNewKey("engineer").setString(m_EngineerId);
    node.getNewKey("state").setInt(m_CheckedState);
    node.getNewKey("checkedDateTime").setString(m_chkdDateTime);
}

bool ChkLstSubItem::setText(QString str)
{
    if(m_Text != str){
        m_Text = str;
        return true;
    }
    return false;
}

bool ChkLstSubItem::setOperator(QString str)
{
    if(m_CheckedState & chkdOperator && !m_OperatorId.isEmpty())
        return m_CheckedState;

    m_OperatorId = str;
    m_CheckedState |= chkdOperator;
    if(m_CheckedState == chkdComplete)
        m_chkdDateTime = QDateTime::currentDateTime().toString("MMM/dd/yyyy hh:mm:ss");
    return m_CheckedState == chkdComplete;

}
bool ChkLstSubItem::setEngineer(QString str)
{
    if(m_CheckedState & chkdEngineer && !m_EngineerId.isEmpty())
        return m_CheckedState;

    m_EngineerId = str;
    m_CheckedState |= chkdEngineer;
    if(m_CheckedState == chkdComplete)
        m_chkdDateTime = QDateTime::currentDateTime().toString("MMM/dd/yyyy hh:mm:ss");
    return m_CheckedState == chkdComplete;
}

/*********************************************************************/
ChkLstItem::ChkLstItem()
{
    m_Text =  "";
    m_Checked =  false;
    m_chkdDateTime =  "";
    m_itemsChecked = 0;
}

ChkLstItem::ChkLstItem(SettingsIONode &node)
{
    m_Text =  node.getNewKey("text").getString();
    m_Checked =  node.getNewKey("checked").getBool();
    m_chkdDateTime =  node.getNewKey("checkedDateTime").getString();
    m_itemsChecked = 0;
    SettingsIONode subitems = node.getNewKey("subitems");
    int index = 0;
    QString key = "subitem%1";
    SettingsIONode subitem_node = subitems.getKey(key.arg(index));
    while(!subitem_node.empty()){
        ChkLstSubItem* subitem = new ChkLstSubItem(subitem_node);
        m_subItems.append(subitem);
        if(subitem->checkState() == 0x3)
            m_itemsChecked++;
        else if(subitem->checkState() > 0)
            m_itemsInProcess = true;
        index++;
        subitem_node = subitems.getKey(key.arg(index));
    }

}

ChkLstItem::~ChkLstItem()
{
    while(m_subItems.count() > 0){
        ChkLstSubItem* subItem = m_subItems.takeAt(0);
        subItem->deleteLater();

    }
}

void ChkLstItem::save(SettingsIONode &node)
{
    node.getNewKey("text").setString(m_Text);
    node.getNewKey("checked").setBool(m_Checked);
    node.getNewKey("checkedDateTime").setString(m_chkdDateTime);
    SettingsIONode subitems = node.getNewKey("subitems");
    for(int i=0; i<m_subItems.count(); i++){
        ChkLstSubItem* subItem = m_subItems.at(i);
        QString key = "subitem%1";
        SettingsIONode subitem_node = subitems.getNewKey(key.arg(i));
        subItem->save(subitem_node);
    }
}

ChkLstSubItem* ChkLstItem::getItem(int index)
{
    if(index < m_subItems.count())
        return m_subItems.at(index);
    return NULL;
}

void ChkLstItem::itemChecked(int state)
{
    if(state = ChkLstSubItem::chkdComplete){
        m_itemsChecked++;
        if(m_itemsChecked == m_subItems.count()){
            m_Checked = true;
            m_chkdDateTime = QDateTime::currentDateTime().toString("MMM-dd-yyyy hh:mm:ss");
        }
    }
    m_itemsInProcess = true;
}

void ChkLstItem::insert(int index, ChkLstSubItem* item)
{
    m_subItems.insert(index, item);
}
void ChkLstItem::append(ChkLstSubItem* item)
{
    m_subItems.append(item);
}
void ChkLstItem::remove(int index)
{
   ChkLstSubItem* subItem = m_subItems.takeAt(index);
   subItem->deleteLater();
}

void ChkLstItem::move(int fromIndex, int toIndex)
{
    m_subItems.move(fromIndex, toIndex);
}

bool ChkLstItem::setText(QString str)
{
    if(m_Text != str){
        m_Text = str;
        return true;
    }
    return false;
}


/*********************************************************************/
ChkLstSubItemModel::ChkLstSubItemModel(QObject *parent)
    :QAbstractListModel(parent)
{
    m_chkLstItem = NULL;
}

QHash<int, QByteArray> ChkLstSubItemModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(ItemIdRole, "itemId");
    roleNames.insert(TextRole, "item_text");
    roleNames.insert(OperatorRole, "item_oper");
    roleNames.insert(EngineerRole, "item_eng");
    roleNames.insert(CheckStateRole, "item_checked");
    roleNames.insert(CheckedDateTimeRole, "item_date");
    return roleNames;
}

void ChkLstSubItemModel::operatorCheck(int _index, QString user)
{
    setData(index(_index,0), user,  OperatorRole);
}

void ChkLstSubItemModel::engineerCheck(int _index, QString user)
{
    setData(index(_index,0), user,  EngineerRole);
}


void ChkLstSubItemModel::load(ChkLstItem* item)
{
    beginResetModel();
    m_chkLstItem = item;
    endResetModel();
}

int ChkLstSubItemModel::rowCount(const QModelIndex & parent) const {
    if(!m_chkLstItem) return (0);

    return m_chkLstItem->itemCount();
}

QVariant ChkLstSubItemModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    if(!m_chkLstItem) QVariant();

    QVariant value;
    ChkLstSubItem* subitem = m_chkLstItem->getItem(index.row());
    if(!subitem) QVariant();

    switch(role){
    case ItemIdRole:{
        QString id = ".%1";
        value = m_itemId + id.arg(index.row() + 1);
        break;
    }
    case TextRole:{
        value = subitem->itemText();
        break;
    }
    case OperatorRole:{
        value = subitem->operatorId();
        break;
    }
    case EngineerRole:{
        value = subitem->engineerId();
        break;
    }
    case CheckStateRole:{
        value = subitem->checkState();
        break;
    }
    case CheckedDateTimeRole:{
        value = subitem->chkdDateTime();
        break;
    }
    default:
        return QVariant();
    }

    return value;
}

bool ChkLstSubItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ( index.column () > 1  || !index.isValid () || index.row () >= rowCount() )
        return false;

    if(!m_chkLstItem) return false;
    ChkLstSubItem* subitem = m_chkLstItem->getItem(index.row());
    if(!subitem) return false;

    switch(role) {
    case TextRole:
    {
        if(subitem->setText(value.toString()))
            emit modified();
        emit dataChanged(index, index);
        break;
    }
    case OperatorRole:
    {
        if(subitem->setOperator(value.toString())){
           m_chkLstItem->itemChecked(subitem->checkState());
        }
        emit dataChanged(index, index);
        emit stateChanged(m_chkLstItem);
        break;
    }
    case EngineerRole:
    {
        if (subitem->setEngineer(value.toString())){
            m_chkLstItem->itemChecked(subitem->checkState());
        }
        emit dataChanged(index, index);
        emit stateChanged(m_chkLstItem);
        break;
    }
    default:
        return false;
    }
    return true;

}

void ChkLstSubItemModel::addItem(int rowIndex)
{
    if(!m_chkLstItem) return;

    int insert_index = rowIndex;
    if(rowIndex < 0){
        insert_index = m_chkLstItem->itemCount();
    }
    beginInsertRows(QModelIndex(), insert_index,insert_index);
    SettingsIONode node;
    m_chkLstItem->insert(insert_index, new ChkLstSubItem(node));
    endInsertRows();
    emit modified();
}

void ChkLstSubItemModel::removeItem(int rowIndex)
{
    if(rowIndex < 0) return;
    beginRemoveRows(QModelIndex(), rowIndex, rowIndex);
    SettingsIONode node;
    m_chkLstItem->remove(rowIndex);
    endRemoveRows();
    emit modified();
}


void ChkLstSubItemModel::moveItem(int fromIndex, int toIndex)
{
    if(fromIndex == toIndex) return;
    int moveIndex = toIndex > fromIndex ? toIndex + 1 : toIndex;
    beginMoveRows(QModelIndex(),fromIndex, fromIndex, QModelIndex(), moveIndex );
    m_chkLstItem->move(fromIndex, toIndex);
    endMoveRows();
    emit modified();
}

/*********************************************************************/
ChkLstItemModel::ChkLstItemModel(QObject *parent)
    :QAbstractListModel(parent)
{
}

QHash<int, QByteArray> ChkLstItemModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(TextRole, "item_text");
    roleNames.insert(CheckedRole, "item_checked");
    roleNames.insert(CheckedDateTimeRole, "item_date");
    return roleNames;
}

void ChkLstItemModel::reset()
{
    while (!m_items.isEmpty())
        m_items.takeFirst()->deleteLater();
}

void ChkLstItemModel::load(SettingsIONode &node)
{
    beginResetModel();
    reset();
    int index = 0;
    QString key = "item%1";
    SettingsIONode item_node = node.getKey(key.arg(index));
    while(!item_node.empty()){
        m_items.append(new ChkLstItem(item_node));
        index++;
        item_node = node.getKey(key.arg(index));
    }

    endResetModel();

    bool complete = true;
    for(int i=0; i < m_items.count(); i++){
        ChkLstItem* _item = m_items.at(i);
        if(!_item->checked())
            complete = false;
    }
    if(complete)
        emit itemsComplete();
}

void ChkLstItemModel::save(SettingsIONode &node)
{
    QString key = "item%1";
    for(int index = 0; index < m_items.count(); index++){
        SettingsIONode item_node = node.getNewKey(key.arg(index));
        ChkLstItem* item = m_items.at(index);
        item->save(item_node);
    }
}

int ChkLstItemModel::rowCount(const QModelIndex & parent) const {

    return m_items.count();
}

QVariant ChkLstItemModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    QVariant value;
    ChkLstItem* item = m_items.at(index.row());
    if(!item) QVariant();

    switch(role){
    case TextRole:{
        value = item->itemText();
        break;
    }
    case CheckedRole:{
        value = item->checked();
        break;
    }
    case CheckedDateTimeRole:{
        value = item->chkdDateTime();
        break;
    }
    default:
        return QVariant();
    }

    return value;
}

bool ChkLstItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ( index.column () > 1  || !index.isValid () || index.row () >= rowCount() )
        return false;

    ChkLstItem* item = m_items.at(index.row());
    if(!item) QVariant();

    switch(role) {
    case TextRole:
    {
        if(item->setText(value.toString()))
            emit modified();
        emit dataChanged(index, index);
        break;
    }
    default:
        return false;
    }
    return true;

}

ChkLstItem* ChkLstItemModel::getItem(int index)
{
    if(index < m_items.count() && index >=0)
        return m_items.at(index);
    return NULL;
}

void ChkLstItemModel::addItem(int rowIndex)
{
    int insert_index = rowIndex;
    if(rowIndex < 0){
        insert_index = m_items.count();
    }

    beginInsertRows(QModelIndex(), insert_index, insert_index);
    ChkLstItem* item = new ChkLstItem();
    ChkLstSubItem* subitem = new ChkLstSubItem();
    item->append(subitem);
    m_items.insert(insert_index, item);
    endInsertRows();
    emit modified();
}

void ChkLstItemModel::removeItem(int rowIndex)
{
    if(rowIndex < 0) return;
    beginRemoveRows(QModelIndex(), rowIndex, rowIndex);
    ChkLstItem* item = m_items.takeAt(rowIndex);
    item->deleteLater();
    endRemoveRows();
    emit modified();
}

void ChkLstItemModel::moveItem(int fromIndex, int toIndex)
{
    if(fromIndex == toIndex) return;
    int moveIndex = toIndex > fromIndex ? toIndex + 1 : toIndex;
    beginMoveRows(QModelIndex(),fromIndex, fromIndex, QModelIndex(), moveIndex );
    m_items.move(fromIndex, toIndex);
    endMoveRows();
    emit modified();
}


void ChkLstItemModel::itemStateChange(ChkLstItem* item)
{
    bool complete = true;
    for(int i=0; i < m_items.count(); i++){
        ChkLstItem* _item = m_items.at(i);
        if(_item == item){
            emit dataChanged(index(i,0), index(i,0));
        }
        if(!_item->checked())
            complete = false;
    }
    if(complete)
        emit itemsComplete();
}


