#ifndef QMLUIPROCESS_H
#define QMLUIPROCESS_H


class QMLUIProcess
{
public:
    QMLUIProcess();

    void PrepQMLDirType();
    void PrepQMLDynamicFields(bool show_values);

    int GetQMLTextIDLink(int field_id);
    int GetQMLDirective(int response_id, int iter);
    int GetQMLDynamicFields(bool get_required, int field_id);

    int QMLDynamicText(int field_id, int dynamic_text_pos);

    int QMLDataCode(int field_id);
    int QMLFieldIDFromDataCode(int data_code);

};

#endif // QMLUIPROCESS_H
