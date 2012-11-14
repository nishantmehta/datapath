dnl #
dnl #  Copyright 2012 Alin Dobra and Christopher Jermaine
dnl #
dnl #  Licensed under the Apache License, Version 2.0 (the "License");
dnl #  you may not use this file except in compliance with the License.
dnl #  You may obtain a copy of the License at
dnl #
dnl #      http://www.apache.org/licenses/LICENSE-2.0
dnl #
dnl #  Unless required by applicable law or agreed to in writing, software
dnl #  distributed under the License is distributed on an "AS IS" BASIS,
dnl #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl #  See the License for the specific language governing permissions and
dnl #  limitations under the License.
dnl #
dnl We assume tha this file is included from Modules.m4 and that all
dnl the m4 libraries needed are loaded
dnl

#include "Dictionary.h"
#include "DictionaryManager.h"

dnl Arguments:
dnl
dnl M4_WPName -- name of the waypoint
dnl
dnl M4_Columns -- list of columns that the bulkloader loads.
dnl   each element is of type (name, type)
dnl   If value is NONE, the attribute is skipped
dnl M4_Separator -- the separator used in the input
dnl
extern "C"
int TextLoaderWorkFunc_<//>M4_WPName (WorkDescription &workDescription, ExecEngineData &result) {
    TextLoaderWorkDescription myWork;
    myWork.swap (workDescription);

    // local dictionary used by all HString datatypes. This is our own dictionary so we do no
    // not need synchronization to update it; Macros use this to let HString types to use it.
    HString::DictionaryWrapper& localDictionaryWrp = myWork.get_localDictionary();
    HString::Dictionary&  localDictionary = localDictionaryWrp.GetDictionary();

dnl # Declare local dictionaries for columns that need it
m4_foreach(</_C_/>, </M4_Columns/>, </dnl
<//>m4_if(_TYPE_REQ_DICT(M4_ATT_TYPE(_C_)), 1, </dnl
    Dictionary _C_</_Local_Dict/>;
<//>/>)dnl
/>)dnl

    // the file we are reading from (state preserved accross calls
    FILE* stream = myWork.get_stream();

<//>M4_GET_QUERIES_TO_RUN(</myWork/>)<//>dnl

    int noTuples=0;
    char* curr;
    char* next;
    bool finished = false; // did we finish

    size_t lineSize=1024; // the size of the line
    char* buffer = (char*)malloc(lineSize); // will be resized by getline

    // Start new columns and allocate storage for them
    // One column/attribute
m4_foreach(</_C_/>, </M4_Columns/>, </dnl
<//><//>M4_DECLARE_COLUMN(_C_)<//>dnl
/>)

    while(true){
        // get next line of text
        // if eof break
        size_t lineRead = getline(&buffer, &lineSize, stream);
        if ( lineRead == -1){
            finished=true; // we read the whole file
            break; // we reached the end of file
        }

        next = buffer;

dnl # M4 CODE
dnl # declaring all the columns
m4_foreach(</_C_/>, </M4_Columns/>, </dnl
        // tokenizing
        curr=next;
        while( (next-buffer)< lineSize && *next != M4_Separator && *next!='\n')
        { next++; }
        *next = 0;
        next++;

<//>m4_case(M4_ATT_TYPE(_C_), NONE, <//*column _C_ skipped *//>, </
        // temporary variable to read the attribute
        M4_BASIC_TYPE(M4_ATT_TYPE(_C_)) _C_<//>Att;
        // populate it from the token
        FromString( _C_<//>Att, curr dnl
m4_if(_TYPE_REQ_DICT(M4_ATT_TYPE(_C_)), 1, </, _C_</_Local_Dict/>/>)<//>dnl
);
        _C_<//>Iterator.Insert(_C_<//>Att);
        _C_<//>Iterator.Advance();
<//>/>)<//>dnl
/>)
dnl # END OF M4 CODE

        noTuples++;

        if (noTuples>=PREFERED_TUPLES_PER_CHUNK)
            break;
    }

    // deallocate the buffer (not needed anymore)
    free(buffer);

    // Deal with the PreDone() calls to release read locks
m4_foreach(</_C_/>, </M4_Columns/>, </dnl
m4_if(_TYPE_REQ_DICT(M4_ATT_TYPE(_C_)), 1, </_C_<//>Iterator.PreDone();/>)
/>)


    // form the chunk and put the columns in it
    Chunk chunk;
    int cSlot=0; // current slot where columns go
    // we use slots 0 to numcols
    SlotContainer slots; // which slots need to be written
m4_foreach(</_C_/>, </M4_Columns/>, </dnl
<//>m4_case(M4_ATT_TYPE(_C_), NONE, <//*column _C_ skipped *//>, </
    {
        SlotID slot(cSlot);
        slots.Append(slot);
    }

    cSlot = M4_ATT_SLOT(_C_);

    _C_<//>Iterator.Done(_C_<//>Column dnl
m4_if(_TYPE_REQ_DICT(M4_ATT_TYPE(_C_)), 1, </, _C_</_Local_Dict/>/>)<//>dnl
);
dnl _C_<//>Column.Compress(false); // compress the
dnl  //column. Keep both compressed and decompressed
    chunk.SwapColumn( _C_<//>Column, cSlot );
<//>/>)<//>dnl
/>)

    MMappedStorage bitStore;
    Column outBitCol(bitStore);
    BStringIterator outQueries (outBitCol, queriesToRun, noTuples );
    outQueries.Done();
    //SS outQueries.Done(outBitCol);
    //SS chunk.SwapBitmap(outBitCol);
    chunk.SwapBitmap(outQueries);

    // pack the chunk, stream and other things in the result and send it back.
    TextLoaderResult tempResult ( stream, noTuples, localDictionaryWrp, chunk);
    tempResult.swap (result);

    if (finished)
        return 1;
    else
        return 0;
}

