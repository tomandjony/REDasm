#ifndef LISTINGDOCUMENTRENDERER_H
#define LISTINGDOCUMENTRENDERER_H

#include "listingrenderercommon.h"

class ListingDocumentRenderer: public ListingRendererCommon
{
    public:
        ListingDocumentRenderer(REDasm::DisassemblerAPI* disassembler);

    protected:
        void renderLine(const REDasm::RendererLine& rl) override;
};

#endif // LISTINGDOCUMENTRENDERER_H
