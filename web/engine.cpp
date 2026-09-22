namespace {
int document_kind = 1;
}

extern "C" int fw_engine_version()
{
    return 1;
}

extern "C" int fw_document_width()
{
    return 1200;
}

extern "C" int fw_document_height()
{
    return 1500;
}

extern "C" int fw_document_kind()
{
    return document_kind;
}

extern "C" int fw_new_document()
{
    document_kind = 0;
    return document_kind;
}

extern "C" int fw_open_starter()
{
    document_kind = 1;
    return document_kind;
}
