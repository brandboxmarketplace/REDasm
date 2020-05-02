#include "gotomodel.h"
#include "../../themeprovider.h"
#include <rdapi/support.h>

GotoModel::GotoModel(QObject *parent) : DisassemblerModel(parent) { }

void GotoModel::setDisassembler(RDDisassembler* disassembler)
{
    this->beginResetModel();
    DisassemblerModel::setDisassembler(disassembler);
    this->endResetModel();
}

QVariant GotoModel::data(const QModelIndex &index, int role) const
{
    if(!m_disassembler) return QVariant();

    RDDocumentItem item;
    RDDocument_GetItemAt(m_document, index.row(), &item);

    if(role == Qt::DisplayRole)
    {
        if(index.column() == 0) return RD_ToHex(item.address);
        if(index.column() == 1) return this->itemName(item);
        if(index.column() == 2) return this->itemType(item);
    }
    else if(role == Qt::TextAlignmentRole)
    {
        if(index.column() == 2) return Qt::AlignCenter;
    }
    else if(role == Qt::ForegroundRole)
    {
        if(index.column() == 0) return THEME_VALUE("address_list_fg");
        if(index.column() == 1) return this->itemColor(item);
    }

    return QVariant();
}

QVariant GotoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        if(section == 0)      return "Address";
        else if(section == 1) return "Name";
        else if(section == 2) return "Type";
    }

    return DisassemblerModel::headerData(section, orientation, role);
}

int GotoModel::columnCount(const QModelIndex &) const { return 3; }
int GotoModel::rowCount(const QModelIndex &) const { return m_document ? RDDocument_ItemsCount(m_document) : 0; }

QColor GotoModel::itemColor(const RDDocumentItem& item) const
{
    RDSymbol symbol;

    switch(item.type)
    {
        case DocumentItemType_Segment:  return THEME_VALUE("segment_fg");
        case DocumentItemType_Function: return THEME_VALUE("function_fg");
        case DocumentItemType_Type:     return THEME_VALUE("type_fg");

        case DocumentItemType_Symbol:
            if(!RDDocument_GetSymbolByAddress(m_document, item.address, &symbol)) return QColor();
            if(symbol.type == SymbolType_String) return THEME_VALUE("string_fg");
            return THEME_VALUE("data_fg");

        default: break;
    }

    return QColor();
}

QString GotoModel::itemName(const RDDocumentItem& item) const
{
    if(item.type == DocumentItemType_Segment)
    {
        RDSegment segment;
        if(RDDocument_GetSegmentAddress(m_document, item.address, &segment)) return segment.name;
    }
    else if((item.type == DocumentItemType_Function) || (item.type == DocumentItemType_Symbol))
    {
        const char* name = RDDocument_GetSymbolName(m_document, item.address);
        if(name) return RD_Demangle(name);
    }
    else if(item.type == DocumentItemType_Type)
        return "TODO"; //TODO: Convert::to_qstring(r_doc->type(item.address));

    return QString();
}

QString GotoModel::itemType(const RDDocumentItem& item) const
{
    switch(item.type)
    {
        case DocumentItemType_Segment:  return "SEGMENT";
        case DocumentItemType_Function: return "FUNCTION";
        case DocumentItemType_Type:     return "TYPE";
        case DocumentItemType_Symbol:   return "SYMBOL";
        default: break;
    }

    return QString();
}
