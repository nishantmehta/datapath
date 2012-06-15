// $ANTLR 3.3 Nov 30, 2010 12:50:56 BaseLexer.g 2012-06-14 15:15:10

import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;

public class BaseLexer extends Lexer {
    public static final int EOF=-1;
    public static final int FILTER=4;
    public static final int JOIN=5;
    public static final int GLA=6;
    public static final int PRINT=7;
    public static final int AGGREGATE=8;
    public static final int EQUAL=9;
    public static final int SEPARATOR=10;
    public static final int ATTRIBUTES=11;
    public static final int TEXTLOADER=12;
    public static final int FILE=13;
    public static final int MATCH_DP=14;
    public static final int CASE_DP=15;
    public static final int BOOL_T=16;
    public static final int ID=17;
    public static final int INT=18;
    public static final int EXPONENT=19;
    public static final int FLOAT=20;
    public static final int COMMENT=21;
    public static final int WS=22;
    public static final int ESC_SEQ=23;
    public static final int STRING=24;
    public static final int SEMICOLON=25;
    public static final int COMMA=26;
    public static final int COLON=27;
    public static final int DOT=28;
    public static final int LPAREN=29;
    public static final int RPAREN=30;
    public static final int LSQ=31;
    public static final int RSQ=32;
    public static final int QMARK=33;
    public static final int LAND=34;
    public static final int LOR=35;
    public static final int BAND=36;
    public static final int BOR=37;
    public static final int XOR=38;
    public static final int LNOT=39;
    public static final int ISEQUAL=40;
    public static final int NEQUAL=41;
    public static final int LS=42;
    public static final int GT=43;
    public static final int LE=44;
    public static final int GE=45;
    public static final int NOT=46;
    public static final int SLEFT=47;
    public static final int SRIGHT=48;
    public static final int PLUS=49;
    public static final int MINUS=50;
    public static final int TIMES=51;
    public static final int DIVIDE=52;
    public static final int MOD=53;
    public static final int HEX_DIGIT=54;
    public static final int UNICODE_ESC=55;
    public static final int OCTAL_ESC=56;
    public static final int OP=57;

    // delegates
    // delegators

    public BaseLexer() {;} 
    public BaseLexer(CharStream input) {
        this(input, new RecognizerSharedState());
    }
    public BaseLexer(CharStream input, RecognizerSharedState state) {
        super(input,state);

    }
    public String getGrammarFileName() { return "BaseLexer.g"; }

    // $ANTLR start "FILTER"
    public final void mFILTER() throws RecognitionException {
        try {
            int _type = FILTER;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:4:9: ( 'filter' | 'Filter' | 'FILTER' )
            int alt1=3;
            int LA1_0 = input.LA(1);

            if ( (LA1_0=='f') ) {
                alt1=1;
            }
            else if ( (LA1_0=='F') ) {
                int LA1_2 = input.LA(2);

                if ( (LA1_2=='i') ) {
                    alt1=2;
                }
                else if ( (LA1_2=='I') ) {
                    alt1=3;
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 1, 2, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 1, 0, input);

                throw nvae;
            }
            switch (alt1) {
                case 1 :
                    // BaseLexer.g:4:13: 'filter'
                    {
                    match("filter"); 


                    }
                    break;
                case 2 :
                    // BaseLexer.g:4:24: 'Filter'
                    {
                    match("Filter"); 


                    }
                    break;
                case 3 :
                    // BaseLexer.g:4:35: 'FILTER'
                    {
                    match("FILTER"); 


                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "FILTER"

    // $ANTLR start "JOIN"
    public final void mJOIN() throws RecognitionException {
        try {
            int _type = JOIN;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:5:9: ( 'join' | 'Join' | 'JOIN' )
            int alt2=3;
            int LA2_0 = input.LA(1);

            if ( (LA2_0=='j') ) {
                alt2=1;
            }
            else if ( (LA2_0=='J') ) {
                int LA2_2 = input.LA(2);

                if ( (LA2_2=='o') ) {
                    alt2=2;
                }
                else if ( (LA2_2=='O') ) {
                    alt2=3;
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 2, 2, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 2, 0, input);

                throw nvae;
            }
            switch (alt2) {
                case 1 :
                    // BaseLexer.g:5:13: 'join'
                    {
                    match("join"); 


                    }
                    break;
                case 2 :
                    // BaseLexer.g:5:22: 'Join'
                    {
                    match("Join"); 


                    }
                    break;
                case 3 :
                    // BaseLexer.g:5:31: 'JOIN'
                    {
                    match("JOIN"); 


                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "JOIN"

    // $ANTLR start "GLA"
    public final void mGLA() throws RecognitionException {
        try {
            int _type = GLA;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:6:9: ( 'gla' | 'Gla' | 'GLA' )
            int alt3=3;
            int LA3_0 = input.LA(1);

            if ( (LA3_0=='g') ) {
                alt3=1;
            }
            else if ( (LA3_0=='G') ) {
                int LA3_2 = input.LA(2);

                if ( (LA3_2=='l') ) {
                    alt3=2;
                }
                else if ( (LA3_2=='L') ) {
                    alt3=3;
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 3, 2, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 3, 0, input);

                throw nvae;
            }
            switch (alt3) {
                case 1 :
                    // BaseLexer.g:6:13: 'gla'
                    {
                    match("gla"); 


                    }
                    break;
                case 2 :
                    // BaseLexer.g:6:21: 'Gla'
                    {
                    match("Gla"); 


                    }
                    break;
                case 3 :
                    // BaseLexer.g:6:29: 'GLA'
                    {
                    match("GLA"); 


                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "GLA"

    // $ANTLR start "PRINT"
    public final void mPRINT() throws RecognitionException {
        try {
            int _type = PRINT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:7:9: ( 'print' | 'Print' | 'PRINT' )
            int alt4=3;
            int LA4_0 = input.LA(1);

            if ( (LA4_0=='p') ) {
                alt4=1;
            }
            else if ( (LA4_0=='P') ) {
                int LA4_2 = input.LA(2);

                if ( (LA4_2=='r') ) {
                    alt4=2;
                }
                else if ( (LA4_2=='R') ) {
                    alt4=3;
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 4, 2, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 4, 0, input);

                throw nvae;
            }
            switch (alt4) {
                case 1 :
                    // BaseLexer.g:7:13: 'print'
                    {
                    match("print"); 


                    }
                    break;
                case 2 :
                    // BaseLexer.g:7:23: 'Print'
                    {
                    match("Print"); 


                    }
                    break;
                case 3 :
                    // BaseLexer.g:7:33: 'PRINT'
                    {
                    match("PRINT"); 


                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "PRINT"

    // $ANTLR start "AGGREGATE"
    public final void mAGGREGATE() throws RecognitionException {
        try {
            int _type = AGGREGATE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:8:11: ( 'aggregate' | 'Aggregate' | 'AGGREGATE' )
            int alt5=3;
            int LA5_0 = input.LA(1);

            if ( (LA5_0=='a') ) {
                alt5=1;
            }
            else if ( (LA5_0=='A') ) {
                int LA5_2 = input.LA(2);

                if ( (LA5_2=='g') ) {
                    alt5=2;
                }
                else if ( (LA5_2=='G') ) {
                    alt5=3;
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 5, 2, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 5, 0, input);

                throw nvae;
            }
            switch (alt5) {
                case 1 :
                    // BaseLexer.g:8:13: 'aggregate'
                    {
                    match("aggregate"); 


                    }
                    break;
                case 2 :
                    // BaseLexer.g:8:27: 'Aggregate'
                    {
                    match("Aggregate"); 


                    }
                    break;
                case 3 :
                    // BaseLexer.g:8:41: 'AGGREGATE'
                    {
                    match("AGGREGATE"); 


                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "AGGREGATE"

    // $ANTLR start "EQUAL"
    public final void mEQUAL() throws RecognitionException {
        try {
            int _type = EQUAL;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:10:9: ( '=' )
            // BaseLexer.g:10:13: '='
            {
            match('='); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "EQUAL"

    // $ANTLR start "SEPARATOR"
    public final void mSEPARATOR() throws RecognitionException {
        try {
            int _type = SEPARATOR;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:12:11: ( 'separator' | 'Separator' | 'SEPARATOR' )
            int alt6=3;
            int LA6_0 = input.LA(1);

            if ( (LA6_0=='s') ) {
                alt6=1;
            }
            else if ( (LA6_0=='S') ) {
                int LA6_2 = input.LA(2);

                if ( (LA6_2=='e') ) {
                    alt6=2;
                }
                else if ( (LA6_2=='E') ) {
                    alt6=3;
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 6, 2, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 6, 0, input);

                throw nvae;
            }
            switch (alt6) {
                case 1 :
                    // BaseLexer.g:12:13: 'separator'
                    {
                    match("separator"); 


                    }
                    break;
                case 2 :
                    // BaseLexer.g:12:27: 'Separator'
                    {
                    match("Separator"); 


                    }
                    break;
                case 3 :
                    // BaseLexer.g:12:41: 'SEPARATOR'
                    {
                    match("SEPARATOR"); 


                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "SEPARATOR"

    // $ANTLR start "ATTRIBUTES"
    public final void mATTRIBUTES() throws RecognitionException {
        try {
            int _type = ATTRIBUTES;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:13:12: ( 'attributes' | 'Attributes' | 'ATTRIBUTES' )
            int alt7=3;
            int LA7_0 = input.LA(1);

            if ( (LA7_0=='a') ) {
                alt7=1;
            }
            else if ( (LA7_0=='A') ) {
                int LA7_2 = input.LA(2);

                if ( (LA7_2=='t') ) {
                    alt7=2;
                }
                else if ( (LA7_2=='T') ) {
                    alt7=3;
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 7, 2, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 7, 0, input);

                throw nvae;
            }
            switch (alt7) {
                case 1 :
                    // BaseLexer.g:13:14: 'attributes'
                    {
                    match("attributes"); 


                    }
                    break;
                case 2 :
                    // BaseLexer.g:13:29: 'Attributes'
                    {
                    match("Attributes"); 


                    }
                    break;
                case 3 :
                    // BaseLexer.g:13:44: 'ATTRIBUTES'
                    {
                    match("ATTRIBUTES"); 


                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "ATTRIBUTES"

    // $ANTLR start "TEXTLOADER"
    public final void mTEXTLOADER() throws RecognitionException {
        try {
            int _type = TEXTLOADER;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:14:12: ( 'textloader' | 'Textloader' | 'TextLoader' | 'TEXTLOADER' )
            int alt8=4;
            int LA8_0 = input.LA(1);

            if ( (LA8_0=='t') ) {
                alt8=1;
            }
            else if ( (LA8_0=='T') ) {
                int LA8_2 = input.LA(2);

                if ( (LA8_2=='e') ) {
                    int LA8_3 = input.LA(3);

                    if ( (LA8_3=='x') ) {
                        int LA8_5 = input.LA(4);

                        if ( (LA8_5=='t') ) {
                            int LA8_6 = input.LA(5);

                            if ( (LA8_6=='l') ) {
                                alt8=2;
                            }
                            else if ( (LA8_6=='L') ) {
                                alt8=3;
                            }
                            else {
                                NoViableAltException nvae =
                                    new NoViableAltException("", 8, 6, input);

                                throw nvae;
                            }
                        }
                        else {
                            NoViableAltException nvae =
                                new NoViableAltException("", 8, 5, input);

                            throw nvae;
                        }
                    }
                    else {
                        NoViableAltException nvae =
                            new NoViableAltException("", 8, 3, input);

                        throw nvae;
                    }
                }
                else if ( (LA8_2=='E') ) {
                    alt8=4;
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 8, 2, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 8, 0, input);

                throw nvae;
            }
            switch (alt8) {
                case 1 :
                    // BaseLexer.g:14:14: 'textloader'
                    {
                    match("textloader"); 


                    }
                    break;
                case 2 :
                    // BaseLexer.g:14:29: 'Textloader'
                    {
                    match("Textloader"); 


                    }
                    break;
                case 3 :
                    // BaseLexer.g:14:44: 'TextLoader'
                    {
                    match("TextLoader"); 


                    }
                    break;
                case 4 :
                    // BaseLexer.g:14:59: 'TEXTLOADER'
                    {
                    match("TEXTLOADER"); 


                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "TEXTLOADER"

    // $ANTLR start "FILE"
    public final void mFILE() throws RecognitionException {
        try {
            int _type = FILE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:15:6: ( 'file' | 'File' | 'FILE' )
            int alt9=3;
            int LA9_0 = input.LA(1);

            if ( (LA9_0=='f') ) {
                alt9=1;
            }
            else if ( (LA9_0=='F') ) {
                int LA9_2 = input.LA(2);

                if ( (LA9_2=='i') ) {
                    alt9=2;
                }
                else if ( (LA9_2=='I') ) {
                    alt9=3;
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 9, 2, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 9, 0, input);

                throw nvae;
            }
            switch (alt9) {
                case 1 :
                    // BaseLexer.g:15:8: 'file'
                    {
                    match("file"); 


                    }
                    break;
                case 2 :
                    // BaseLexer.g:15:17: 'File'
                    {
                    match("File"); 


                    }
                    break;
                case 3 :
                    // BaseLexer.g:15:26: 'FILE'
                    {
                    match("FILE"); 


                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "FILE"

    // $ANTLR start "MATCH_DP"
    public final void mMATCH_DP() throws RecognitionException {
        try {
            int _type = MATCH_DP;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:23:10: ( 'match' | 'Match' | 'MATCH' )
            int alt10=3;
            int LA10_0 = input.LA(1);

            if ( (LA10_0=='m') ) {
                alt10=1;
            }
            else if ( (LA10_0=='M') ) {
                int LA10_2 = input.LA(2);

                if ( (LA10_2=='a') ) {
                    alt10=2;
                }
                else if ( (LA10_2=='A') ) {
                    alt10=3;
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 10, 2, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 10, 0, input);

                throw nvae;
            }
            switch (alt10) {
                case 1 :
                    // BaseLexer.g:23:12: 'match'
                    {
                    match("match"); 


                    }
                    break;
                case 2 :
                    // BaseLexer.g:23:22: 'Match'
                    {
                    match("Match"); 


                    }
                    break;
                case 3 :
                    // BaseLexer.g:23:32: 'MATCH'
                    {
                    match("MATCH"); 


                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "MATCH_DP"

    // $ANTLR start "CASE_DP"
    public final void mCASE_DP() throws RecognitionException {
        try {
            int _type = CASE_DP;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:28:9: ( 'case' | 'Case' | 'CASE' )
            int alt11=3;
            int LA11_0 = input.LA(1);

            if ( (LA11_0=='c') ) {
                alt11=1;
            }
            else if ( (LA11_0=='C') ) {
                int LA11_2 = input.LA(2);

                if ( (LA11_2=='a') ) {
                    alt11=2;
                }
                else if ( (LA11_2=='A') ) {
                    alt11=3;
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 11, 2, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 11, 0, input);

                throw nvae;
            }
            switch (alt11) {
                case 1 :
                    // BaseLexer.g:28:13: 'case'
                    {
                    match("case"); 


                    }
                    break;
                case 2 :
                    // BaseLexer.g:28:22: 'Case'
                    {
                    match("Case"); 


                    }
                    break;
                case 3 :
                    // BaseLexer.g:28:31: 'CASE'
                    {
                    match("CASE"); 


                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "CASE_DP"

    // $ANTLR start "BOOL_T"
    public final void mBOOL_T() throws RecognitionException {
        try {
            int _type = BOOL_T;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:32:8: ( 'true' | 'false' )
            int alt12=2;
            int LA12_0 = input.LA(1);

            if ( (LA12_0=='t') ) {
                alt12=1;
            }
            else if ( (LA12_0=='f') ) {
                alt12=2;
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 12, 0, input);

                throw nvae;
            }
            switch (alt12) {
                case 1 :
                    // BaseLexer.g:32:10: 'true'
                    {
                    match("true"); 


                    }
                    break;
                case 2 :
                    // BaseLexer.g:32:19: 'false'
                    {
                    match("false"); 


                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "BOOL_T"

    // $ANTLR start "ID"
    public final void mID() throws RecognitionException {
        try {
            int _type = ID;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:34:5: ( ( 'a' .. 'z' | 'A' .. 'Z' | '_' ) ( 'a' .. 'z' | 'A' .. 'Z' | '0' .. '9' | '_' )* )
            // BaseLexer.g:34:9: ( 'a' .. 'z' | 'A' .. 'Z' | '_' ) ( 'a' .. 'z' | 'A' .. 'Z' | '0' .. '9' | '_' )*
            {
            if ( (input.LA(1)>='A' && input.LA(1)<='Z')||input.LA(1)=='_'||(input.LA(1)>='a' && input.LA(1)<='z') ) {
                input.consume();

            }
            else {
                MismatchedSetException mse = new MismatchedSetException(null,input);
                recover(mse);
                throw mse;}

            // BaseLexer.g:34:33: ( 'a' .. 'z' | 'A' .. 'Z' | '0' .. '9' | '_' )*
            loop13:
            do {
                int alt13=2;
                int LA13_0 = input.LA(1);

                if ( ((LA13_0>='0' && LA13_0<='9')||(LA13_0>='A' && LA13_0<='Z')||LA13_0=='_'||(LA13_0>='a' && LA13_0<='z')) ) {
                    alt13=1;
                }


                switch (alt13) {
            	case 1 :
            	    // BaseLexer.g:
            	    {
            	    if ( (input.LA(1)>='0' && input.LA(1)<='9')||(input.LA(1)>='A' && input.LA(1)<='Z')||input.LA(1)=='_'||(input.LA(1)>='a' && input.LA(1)<='z') ) {
            	        input.consume();

            	    }
            	    else {
            	        MismatchedSetException mse = new MismatchedSetException(null,input);
            	        recover(mse);
            	        throw mse;}


            	    }
            	    break;

            	default :
            	    break loop13;
                }
            } while (true);


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "ID"

    // $ANTLR start "INT"
    public final void mINT() throws RecognitionException {
        try {
            int _type = INT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:37:5: ( ( '0' .. '9' )+ ( 'L' )? )
            // BaseLexer.g:37:9: ( '0' .. '9' )+ ( 'L' )?
            {
            // BaseLexer.g:37:9: ( '0' .. '9' )+
            int cnt14=0;
            loop14:
            do {
                int alt14=2;
                int LA14_0 = input.LA(1);

                if ( ((LA14_0>='0' && LA14_0<='9')) ) {
                    alt14=1;
                }


                switch (alt14) {
            	case 1 :
            	    // BaseLexer.g:37:9: '0' .. '9'
            	    {
            	    matchRange('0','9'); 

            	    }
            	    break;

            	default :
            	    if ( cnt14 >= 1 ) break loop14;
                        EarlyExitException eee =
                            new EarlyExitException(14, input);
                        throw eee;
                }
                cnt14++;
            } while (true);

            // BaseLexer.g:37:19: ( 'L' )?
            int alt15=2;
            int LA15_0 = input.LA(1);

            if ( (LA15_0=='L') ) {
                alt15=1;
            }
            switch (alt15) {
                case 1 :
                    // BaseLexer.g:37:19: 'L'
                    {
                    match('L'); 

                    }
                    break;

            }


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "INT"

    // $ANTLR start "FLOAT"
    public final void mFLOAT() throws RecognitionException {
        try {
            int _type = FLOAT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:41:5: ( ( '0' .. '9' )+ '.' ( '0' .. '9' )* ( EXPONENT )? ( 'f' | 'L' )? | '.' ( '0' .. '9' )+ ( EXPONENT )? ( 'f' | 'L' )? | ( '0' .. '9' )+ EXPONENT ( 'f' | 'L' )? )
            int alt25=3;
            alt25 = dfa25.predict(input);
            switch (alt25) {
                case 1 :
                    // BaseLexer.g:41:9: ( '0' .. '9' )+ '.' ( '0' .. '9' )* ( EXPONENT )? ( 'f' | 'L' )?
                    {
                    // BaseLexer.g:41:9: ( '0' .. '9' )+
                    int cnt16=0;
                    loop16:
                    do {
                        int alt16=2;
                        int LA16_0 = input.LA(1);

                        if ( ((LA16_0>='0' && LA16_0<='9')) ) {
                            alt16=1;
                        }


                        switch (alt16) {
                    	case 1 :
                    	    // BaseLexer.g:41:10: '0' .. '9'
                    	    {
                    	    matchRange('0','9'); 

                    	    }
                    	    break;

                    	default :
                    	    if ( cnt16 >= 1 ) break loop16;
                                EarlyExitException eee =
                                    new EarlyExitException(16, input);
                                throw eee;
                        }
                        cnt16++;
                    } while (true);

                    match('.'); 
                    // BaseLexer.g:41:25: ( '0' .. '9' )*
                    loop17:
                    do {
                        int alt17=2;
                        int LA17_0 = input.LA(1);

                        if ( ((LA17_0>='0' && LA17_0<='9')) ) {
                            alt17=1;
                        }


                        switch (alt17) {
                    	case 1 :
                    	    // BaseLexer.g:41:26: '0' .. '9'
                    	    {
                    	    matchRange('0','9'); 

                    	    }
                    	    break;

                    	default :
                    	    break loop17;
                        }
                    } while (true);

                    // BaseLexer.g:41:37: ( EXPONENT )?
                    int alt18=2;
                    int LA18_0 = input.LA(1);

                    if ( (LA18_0=='E'||LA18_0=='e') ) {
                        alt18=1;
                    }
                    switch (alt18) {
                        case 1 :
                            // BaseLexer.g:41:37: EXPONENT
                            {
                            mEXPONENT(); 

                            }
                            break;

                    }

                    // BaseLexer.g:41:47: ( 'f' | 'L' )?
                    int alt19=2;
                    int LA19_0 = input.LA(1);

                    if ( (LA19_0=='L'||LA19_0=='f') ) {
                        alt19=1;
                    }
                    switch (alt19) {
                        case 1 :
                            // BaseLexer.g:
                            {
                            if ( input.LA(1)=='L'||input.LA(1)=='f' ) {
                                input.consume();

                            }
                            else {
                                MismatchedSetException mse = new MismatchedSetException(null,input);
                                recover(mse);
                                throw mse;}


                            }
                            break;

                    }


                    }
                    break;
                case 2 :
                    // BaseLexer.g:42:9: '.' ( '0' .. '9' )+ ( EXPONENT )? ( 'f' | 'L' )?
                    {
                    match('.'); 
                    // BaseLexer.g:42:13: ( '0' .. '9' )+
                    int cnt20=0;
                    loop20:
                    do {
                        int alt20=2;
                        int LA20_0 = input.LA(1);

                        if ( ((LA20_0>='0' && LA20_0<='9')) ) {
                            alt20=1;
                        }


                        switch (alt20) {
                    	case 1 :
                    	    // BaseLexer.g:42:14: '0' .. '9'
                    	    {
                    	    matchRange('0','9'); 

                    	    }
                    	    break;

                    	default :
                    	    if ( cnt20 >= 1 ) break loop20;
                                EarlyExitException eee =
                                    new EarlyExitException(20, input);
                                throw eee;
                        }
                        cnt20++;
                    } while (true);

                    // BaseLexer.g:42:25: ( EXPONENT )?
                    int alt21=2;
                    int LA21_0 = input.LA(1);

                    if ( (LA21_0=='E'||LA21_0=='e') ) {
                        alt21=1;
                    }
                    switch (alt21) {
                        case 1 :
                            // BaseLexer.g:42:25: EXPONENT
                            {
                            mEXPONENT(); 

                            }
                            break;

                    }

                    // BaseLexer.g:42:35: ( 'f' | 'L' )?
                    int alt22=2;
                    int LA22_0 = input.LA(1);

                    if ( (LA22_0=='L'||LA22_0=='f') ) {
                        alt22=1;
                    }
                    switch (alt22) {
                        case 1 :
                            // BaseLexer.g:
                            {
                            if ( input.LA(1)=='L'||input.LA(1)=='f' ) {
                                input.consume();

                            }
                            else {
                                MismatchedSetException mse = new MismatchedSetException(null,input);
                                recover(mse);
                                throw mse;}


                            }
                            break;

                    }


                    }
                    break;
                case 3 :
                    // BaseLexer.g:43:9: ( '0' .. '9' )+ EXPONENT ( 'f' | 'L' )?
                    {
                    // BaseLexer.g:43:9: ( '0' .. '9' )+
                    int cnt23=0;
                    loop23:
                    do {
                        int alt23=2;
                        int LA23_0 = input.LA(1);

                        if ( ((LA23_0>='0' && LA23_0<='9')) ) {
                            alt23=1;
                        }


                        switch (alt23) {
                    	case 1 :
                    	    // BaseLexer.g:43:10: '0' .. '9'
                    	    {
                    	    matchRange('0','9'); 

                    	    }
                    	    break;

                    	default :
                    	    if ( cnt23 >= 1 ) break loop23;
                                EarlyExitException eee =
                                    new EarlyExitException(23, input);
                                throw eee;
                        }
                        cnt23++;
                    } while (true);

                    mEXPONENT(); 
                    // BaseLexer.g:43:30: ( 'f' | 'L' )?
                    int alt24=2;
                    int LA24_0 = input.LA(1);

                    if ( (LA24_0=='L'||LA24_0=='f') ) {
                        alt24=1;
                    }
                    switch (alt24) {
                        case 1 :
                            // BaseLexer.g:
                            {
                            if ( input.LA(1)=='L'||input.LA(1)=='f' ) {
                                input.consume();

                            }
                            else {
                                MismatchedSetException mse = new MismatchedSetException(null,input);
                                recover(mse);
                                throw mse;}


                            }
                            break;

                    }


                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "FLOAT"

    // $ANTLR start "COMMENT"
    public final void mCOMMENT() throws RecognitionException {
        try {
            int _type = COMMENT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:47:5: ( '//' (~ ( '\\n' | '\\r' ) )* ( '\\r' )? '\\n' | '/*' ( options {greedy=false; } : . )* '*/' )
            int alt29=2;
            int LA29_0 = input.LA(1);

            if ( (LA29_0=='/') ) {
                int LA29_1 = input.LA(2);

                if ( (LA29_1=='/') ) {
                    alt29=1;
                }
                else if ( (LA29_1=='*') ) {
                    alt29=2;
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 29, 1, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 29, 0, input);

                throw nvae;
            }
            switch (alt29) {
                case 1 :
                    // BaseLexer.g:47:9: '//' (~ ( '\\n' | '\\r' ) )* ( '\\r' )? '\\n'
                    {
                    match("//"); 

                    // BaseLexer.g:47:14: (~ ( '\\n' | '\\r' ) )*
                    loop26:
                    do {
                        int alt26=2;
                        int LA26_0 = input.LA(1);

                        if ( ((LA26_0>='\u0000' && LA26_0<='\t')||(LA26_0>='\u000B' && LA26_0<='\f')||(LA26_0>='\u000E' && LA26_0<='\uFFFF')) ) {
                            alt26=1;
                        }


                        switch (alt26) {
                    	case 1 :
                    	    // BaseLexer.g:47:14: ~ ( '\\n' | '\\r' )
                    	    {
                    	    if ( (input.LA(1)>='\u0000' && input.LA(1)<='\t')||(input.LA(1)>='\u000B' && input.LA(1)<='\f')||(input.LA(1)>='\u000E' && input.LA(1)<='\uFFFF') ) {
                    	        input.consume();

                    	    }
                    	    else {
                    	        MismatchedSetException mse = new MismatchedSetException(null,input);
                    	        recover(mse);
                    	        throw mse;}


                    	    }
                    	    break;

                    	default :
                    	    break loop26;
                        }
                    } while (true);

                    // BaseLexer.g:47:28: ( '\\r' )?
                    int alt27=2;
                    int LA27_0 = input.LA(1);

                    if ( (LA27_0=='\r') ) {
                        alt27=1;
                    }
                    switch (alt27) {
                        case 1 :
                            // BaseLexer.g:47:28: '\\r'
                            {
                            match('\r'); 

                            }
                            break;

                    }

                    match('\n'); 
                    _channel=HIDDEN;

                    }
                    break;
                case 2 :
                    // BaseLexer.g:48:9: '/*' ( options {greedy=false; } : . )* '*/'
                    {
                    match("/*"); 

                    // BaseLexer.g:48:14: ( options {greedy=false; } : . )*
                    loop28:
                    do {
                        int alt28=2;
                        int LA28_0 = input.LA(1);

                        if ( (LA28_0=='*') ) {
                            int LA28_1 = input.LA(2);

                            if ( (LA28_1=='/') ) {
                                alt28=2;
                            }
                            else if ( ((LA28_1>='\u0000' && LA28_1<='.')||(LA28_1>='0' && LA28_1<='\uFFFF')) ) {
                                alt28=1;
                            }


                        }
                        else if ( ((LA28_0>='\u0000' && LA28_0<=')')||(LA28_0>='+' && LA28_0<='\uFFFF')) ) {
                            alt28=1;
                        }


                        switch (alt28) {
                    	case 1 :
                    	    // BaseLexer.g:48:42: .
                    	    {
                    	    matchAny(); 

                    	    }
                    	    break;

                    	default :
                    	    break loop28;
                        }
                    } while (true);

                    match("*/"); 

                    _channel=HIDDEN;

                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "COMMENT"

    // $ANTLR start "WS"
    public final void mWS() throws RecognitionException {
        try {
            int _type = WS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:51:5: ( ( ' ' | '\\t' | '\\r' | '\\n' ) )
            // BaseLexer.g:51:9: ( ' ' | '\\t' | '\\r' | '\\n' )
            {
            if ( (input.LA(1)>='\t' && input.LA(1)<='\n')||input.LA(1)=='\r'||input.LA(1)==' ' ) {
                input.consume();

            }
            else {
                MismatchedSetException mse = new MismatchedSetException(null,input);
                recover(mse);
                throw mse;}

            _channel=HIDDEN;

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "WS"

    // $ANTLR start "STRING"
    public final void mSTRING() throws RecognitionException {
        try {
            int _type = STRING;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:59:5: ( '\\'' ( ESC_SEQ | ~ ( '\\'' | '\\\\' ) )* '\\'' | '\"' ( ESC_SEQ | ~ ( '\\\\' | '\"' ) )* '\"' )
            int alt32=2;
            int LA32_0 = input.LA(1);

            if ( (LA32_0=='\'') ) {
                alt32=1;
            }
            else if ( (LA32_0=='\"') ) {
                alt32=2;
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 32, 0, input);

                throw nvae;
            }
            switch (alt32) {
                case 1 :
                    // BaseLexer.g:59:8: '\\'' ( ESC_SEQ | ~ ( '\\'' | '\\\\' ) )* '\\''
                    {
                    match('\''); 
                    // BaseLexer.g:59:13: ( ESC_SEQ | ~ ( '\\'' | '\\\\' ) )*
                    loop30:
                    do {
                        int alt30=3;
                        int LA30_0 = input.LA(1);

                        if ( (LA30_0=='\\') ) {
                            alt30=1;
                        }
                        else if ( ((LA30_0>='\u0000' && LA30_0<='&')||(LA30_0>='(' && LA30_0<='[')||(LA30_0>=']' && LA30_0<='\uFFFF')) ) {
                            alt30=2;
                        }


                        switch (alt30) {
                    	case 1 :
                    	    // BaseLexer.g:59:15: ESC_SEQ
                    	    {
                    	    mESC_SEQ(); 

                    	    }
                    	    break;
                    	case 2 :
                    	    // BaseLexer.g:59:25: ~ ( '\\'' | '\\\\' )
                    	    {
                    	    if ( (input.LA(1)>='\u0000' && input.LA(1)<='&')||(input.LA(1)>='(' && input.LA(1)<='[')||(input.LA(1)>=']' && input.LA(1)<='\uFFFF') ) {
                    	        input.consume();

                    	    }
                    	    else {
                    	        MismatchedSetException mse = new MismatchedSetException(null,input);
                    	        recover(mse);
                    	        throw mse;}


                    	    }
                    	    break;

                    	default :
                    	    break loop30;
                        }
                    } while (true);

                    match('\''); 

                    }
                    break;
                case 2 :
                    // BaseLexer.g:60:8: '\"' ( ESC_SEQ | ~ ( '\\\\' | '\"' ) )* '\"'
                    {
                    match('\"'); 
                    // BaseLexer.g:60:12: ( ESC_SEQ | ~ ( '\\\\' | '\"' ) )*
                    loop31:
                    do {
                        int alt31=3;
                        int LA31_0 = input.LA(1);

                        if ( (LA31_0=='\\') ) {
                            alt31=1;
                        }
                        else if ( ((LA31_0>='\u0000' && LA31_0<='!')||(LA31_0>='#' && LA31_0<='[')||(LA31_0>=']' && LA31_0<='\uFFFF')) ) {
                            alt31=2;
                        }


                        switch (alt31) {
                    	case 1 :
                    	    // BaseLexer.g:60:14: ESC_SEQ
                    	    {
                    	    mESC_SEQ(); 

                    	    }
                    	    break;
                    	case 2 :
                    	    // BaseLexer.g:60:24: ~ ( '\\\\' | '\"' )
                    	    {
                    	    if ( (input.LA(1)>='\u0000' && input.LA(1)<='!')||(input.LA(1)>='#' && input.LA(1)<='[')||(input.LA(1)>=']' && input.LA(1)<='\uFFFF') ) {
                    	        input.consume();

                    	    }
                    	    else {
                    	        MismatchedSetException mse = new MismatchedSetException(null,input);
                    	        recover(mse);
                    	        throw mse;}


                    	    }
                    	    break;

                    	default :
                    	    break loop31;
                        }
                    } while (true);

                    match('\"'); 

                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "STRING"

    // $ANTLR start "SEMICOLON"
    public final void mSEMICOLON() throws RecognitionException {
        try {
            int _type = SEMICOLON;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:63:11: ( ';' )
            // BaseLexer.g:63:13: ';'
            {
            match(';'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "SEMICOLON"

    // $ANTLR start "COMMA"
    public final void mCOMMA() throws RecognitionException {
        try {
            int _type = COMMA;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:64:7: ( ',' )
            // BaseLexer.g:64:9: ','
            {
            match(','); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "COMMA"

    // $ANTLR start "COLON"
    public final void mCOLON() throws RecognitionException {
        try {
            int _type = COLON;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:65:7: ( ':' )
            // BaseLexer.g:65:9: ':'
            {
            match(':'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "COLON"

    // $ANTLR start "DOT"
    public final void mDOT() throws RecognitionException {
        try {
            int _type = DOT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:66:5: ( '.' )
            // BaseLexer.g:66:7: '.'
            {
            match('.'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "DOT"

    // $ANTLR start "LPAREN"
    public final void mLPAREN() throws RecognitionException {
        try {
            int _type = LPAREN;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:67:8: ( '(' )
            // BaseLexer.g:67:10: '('
            {
            match('('); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "LPAREN"

    // $ANTLR start "RPAREN"
    public final void mRPAREN() throws RecognitionException {
        try {
            int _type = RPAREN;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:68:8: ( ')' )
            // BaseLexer.g:68:10: ')'
            {
            match(')'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "RPAREN"

    // $ANTLR start "LSQ"
    public final void mLSQ() throws RecognitionException {
        try {
            int _type = LSQ;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:69:5: ( '[' )
            // BaseLexer.g:69:7: '['
            {
            match('['); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "LSQ"

    // $ANTLR start "RSQ"
    public final void mRSQ() throws RecognitionException {
        try {
            int _type = RSQ;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:70:5: ( ']' )
            // BaseLexer.g:70:7: ']'
            {
            match(']'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "RSQ"

    // $ANTLR start "QMARK"
    public final void mQMARK() throws RecognitionException {
        try {
            int _type = QMARK;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:71:7: ( '?' )
            // BaseLexer.g:71:9: '?'
            {
            match('?'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "QMARK"

    // $ANTLR start "LAND"
    public final void mLAND() throws RecognitionException {
        try {
            int _type = LAND;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:74:6: ( '&&' )
            // BaseLexer.g:74:8: '&&'
            {
            match("&&"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "LAND"

    // $ANTLR start "LOR"
    public final void mLOR() throws RecognitionException {
        try {
            int _type = LOR;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:75:5: ( '||' )
            // BaseLexer.g:75:7: '||'
            {
            match("||"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "LOR"

    // $ANTLR start "BAND"
    public final void mBAND() throws RecognitionException {
        try {
            int _type = BAND;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:76:6: ( '&' )
            // BaseLexer.g:76:8: '&'
            {
            match('&'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "BAND"

    // $ANTLR start "BOR"
    public final void mBOR() throws RecognitionException {
        try {
            int _type = BOR;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:77:5: ( '|' )
            // BaseLexer.g:77:7: '|'
            {
            match('|'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "BOR"

    // $ANTLR start "XOR"
    public final void mXOR() throws RecognitionException {
        try {
            int _type = XOR;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:78:5: ( '^' )
            // BaseLexer.g:78:7: '^'
            {
            match('^'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "XOR"

    // $ANTLR start "LNOT"
    public final void mLNOT() throws RecognitionException {
        try {
            int _type = LNOT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:79:6: ( '~' )
            // BaseLexer.g:79:8: '~'
            {
            match('~'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "LNOT"

    // $ANTLR start "ISEQUAL"
    public final void mISEQUAL() throws RecognitionException {
        try {
            int _type = ISEQUAL;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:82:9: ( '==' )
            // BaseLexer.g:82:11: '=='
            {
            match("=="); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "ISEQUAL"

    // $ANTLR start "NEQUAL"
    public final void mNEQUAL() throws RecognitionException {
        try {
            int _type = NEQUAL;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:83:8: ( '!=' )
            // BaseLexer.g:83:10: '!='
            {
            match("!="); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "NEQUAL"

    // $ANTLR start "LS"
    public final void mLS() throws RecognitionException {
        try {
            int _type = LS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:84:4: ( '<' )
            // BaseLexer.g:84:6: '<'
            {
            match('<'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "LS"

    // $ANTLR start "GT"
    public final void mGT() throws RecognitionException {
        try {
            int _type = GT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:85:4: ( '>' )
            // BaseLexer.g:85:6: '>'
            {
            match('>'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "GT"

    // $ANTLR start "LE"
    public final void mLE() throws RecognitionException {
        try {
            int _type = LE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:86:4: ( '<=' )
            // BaseLexer.g:86:6: '<='
            {
            match("<="); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "LE"

    // $ANTLR start "GE"
    public final void mGE() throws RecognitionException {
        try {
            int _type = GE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:87:4: ( '>=' )
            // BaseLexer.g:87:6: '>='
            {
            match(">="); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "GE"

    // $ANTLR start "NOT"
    public final void mNOT() throws RecognitionException {
        try {
            int _type = NOT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:88:5: ( '!' )
            // BaseLexer.g:88:7: '!'
            {
            match('!'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "NOT"

    // $ANTLR start "SLEFT"
    public final void mSLEFT() throws RecognitionException {
        try {
            int _type = SLEFT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:91:7: ( '<<' )
            // BaseLexer.g:91:9: '<<'
            {
            match("<<"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "SLEFT"

    // $ANTLR start "SRIGHT"
    public final void mSRIGHT() throws RecognitionException {
        try {
            int _type = SRIGHT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:92:8: ( '>>' )
            // BaseLexer.g:92:10: '>>'
            {
            match(">>"); 


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "SRIGHT"

    // $ANTLR start "PLUS"
    public final void mPLUS() throws RecognitionException {
        try {
            int _type = PLUS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:93:6: ( '+' )
            // BaseLexer.g:93:8: '+'
            {
            match('+'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "PLUS"

    // $ANTLR start "MINUS"
    public final void mMINUS() throws RecognitionException {
        try {
            int _type = MINUS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:94:7: ( '-' )
            // BaseLexer.g:94:9: '-'
            {
            match('-'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "MINUS"

    // $ANTLR start "TIMES"
    public final void mTIMES() throws RecognitionException {
        try {
            int _type = TIMES;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:95:7: ( '*' )
            // BaseLexer.g:95:9: '*'
            {
            match('*'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "TIMES"

    // $ANTLR start "DIVIDE"
    public final void mDIVIDE() throws RecognitionException {
        try {
            int _type = DIVIDE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:96:8: ( '/' )
            // BaseLexer.g:96:10: '/'
            {
            match('/'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "DIVIDE"

    // $ANTLR start "MOD"
    public final void mMOD() throws RecognitionException {
        try {
            int _type = MOD;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // BaseLexer.g:97:5: ( '%' )
            // BaseLexer.g:97:7: '%'
            {
            match('%'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        }
    }
    // $ANTLR end "MOD"

    // $ANTLR start "EXPONENT"
    public final void mEXPONENT() throws RecognitionException {
        try {
            // BaseLexer.g:100:10: ( ( 'e' | 'E' ) ( '+' | '-' )? ( '0' .. '9' )+ )
            // BaseLexer.g:100:12: ( 'e' | 'E' ) ( '+' | '-' )? ( '0' .. '9' )+
            {
            if ( input.LA(1)=='E'||input.LA(1)=='e' ) {
                input.consume();

            }
            else {
                MismatchedSetException mse = new MismatchedSetException(null,input);
                recover(mse);
                throw mse;}

            // BaseLexer.g:100:22: ( '+' | '-' )?
            int alt33=2;
            int LA33_0 = input.LA(1);

            if ( (LA33_0=='+'||LA33_0=='-') ) {
                alt33=1;
            }
            switch (alt33) {
                case 1 :
                    // BaseLexer.g:
                    {
                    if ( input.LA(1)=='+'||input.LA(1)=='-' ) {
                        input.consume();

                    }
                    else {
                        MismatchedSetException mse = new MismatchedSetException(null,input);
                        recover(mse);
                        throw mse;}


                    }
                    break;

            }

            // BaseLexer.g:100:33: ( '0' .. '9' )+
            int cnt34=0;
            loop34:
            do {
                int alt34=2;
                int LA34_0 = input.LA(1);

                if ( ((LA34_0>='0' && LA34_0<='9')) ) {
                    alt34=1;
                }


                switch (alt34) {
            	case 1 :
            	    // BaseLexer.g:100:34: '0' .. '9'
            	    {
            	    matchRange('0','9'); 

            	    }
            	    break;

            	default :
            	    if ( cnt34 >= 1 ) break loop34;
                        EarlyExitException eee =
                            new EarlyExitException(34, input);
                        throw eee;
                }
                cnt34++;
            } while (true);


            }

        }
        finally {
        }
    }
    // $ANTLR end "EXPONENT"

    // $ANTLR start "HEX_DIGIT"
    public final void mHEX_DIGIT() throws RecognitionException {
        try {
            // BaseLexer.g:103:11: ( ( '0' .. '9' | 'a' .. 'f' | 'A' .. 'F' ) )
            // BaseLexer.g:103:13: ( '0' .. '9' | 'a' .. 'f' | 'A' .. 'F' )
            {
            if ( (input.LA(1)>='0' && input.LA(1)<='9')||(input.LA(1)>='A' && input.LA(1)<='F')||(input.LA(1)>='a' && input.LA(1)<='f') ) {
                input.consume();

            }
            else {
                MismatchedSetException mse = new MismatchedSetException(null,input);
                recover(mse);
                throw mse;}


            }

        }
        finally {
        }
    }
    // $ANTLR end "HEX_DIGIT"

    // $ANTLR start "ESC_SEQ"
    public final void mESC_SEQ() throws RecognitionException {
        try {
            // BaseLexer.g:107:5: ( '\\\\' ( 'b' | 't' | 'n' | 'f' | 'r' | '\\\"' | '\\'' | '\\\\' ) | UNICODE_ESC | OCTAL_ESC )
            int alt35=3;
            int LA35_0 = input.LA(1);

            if ( (LA35_0=='\\') ) {
                switch ( input.LA(2) ) {
                case '\"':
                case '\'':
                case '\\':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                    {
                    alt35=1;
                    }
                    break;
                case 'u':
                    {
                    alt35=2;
                    }
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                    {
                    alt35=3;
                    }
                    break;
                default:
                    NoViableAltException nvae =
                        new NoViableAltException("", 35, 1, input);

                    throw nvae;
                }

            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 35, 0, input);

                throw nvae;
            }
            switch (alt35) {
                case 1 :
                    // BaseLexer.g:107:9: '\\\\' ( 'b' | 't' | 'n' | 'f' | 'r' | '\\\"' | '\\'' | '\\\\' )
                    {
                    match('\\'); 
                    if ( input.LA(1)=='\"'||input.LA(1)=='\''||input.LA(1)=='\\'||input.LA(1)=='b'||input.LA(1)=='f'||input.LA(1)=='n'||input.LA(1)=='r'||input.LA(1)=='t' ) {
                        input.consume();

                    }
                    else {
                        MismatchedSetException mse = new MismatchedSetException(null,input);
                        recover(mse);
                        throw mse;}


                    }
                    break;
                case 2 :
                    // BaseLexer.g:108:9: UNICODE_ESC
                    {
                    mUNICODE_ESC(); 

                    }
                    break;
                case 3 :
                    // BaseLexer.g:109:9: OCTAL_ESC
                    {
                    mOCTAL_ESC(); 

                    }
                    break;

            }
        }
        finally {
        }
    }
    // $ANTLR end "ESC_SEQ"

    // $ANTLR start "OCTAL_ESC"
    public final void mOCTAL_ESC() throws RecognitionException {
        try {
            // BaseLexer.g:114:5: ( '\\\\' ( '0' .. '3' ) ( '0' .. '7' ) ( '0' .. '7' ) | '\\\\' ( '0' .. '7' ) ( '0' .. '7' ) | '\\\\' ( '0' .. '7' ) )
            int alt36=3;
            int LA36_0 = input.LA(1);

            if ( (LA36_0=='\\') ) {
                int LA36_1 = input.LA(2);

                if ( ((LA36_1>='0' && LA36_1<='3')) ) {
                    int LA36_2 = input.LA(3);

                    if ( ((LA36_2>='0' && LA36_2<='7')) ) {
                        int LA36_4 = input.LA(4);

                        if ( ((LA36_4>='0' && LA36_4<='7')) ) {
                            alt36=1;
                        }
                        else {
                            alt36=2;}
                    }
                    else {
                        alt36=3;}
                }
                else if ( ((LA36_1>='4' && LA36_1<='7')) ) {
                    int LA36_3 = input.LA(3);

                    if ( ((LA36_3>='0' && LA36_3<='7')) ) {
                        alt36=2;
                    }
                    else {
                        alt36=3;}
                }
                else {
                    NoViableAltException nvae =
                        new NoViableAltException("", 36, 1, input);

                    throw nvae;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 36, 0, input);

                throw nvae;
            }
            switch (alt36) {
                case 1 :
                    // BaseLexer.g:114:9: '\\\\' ( '0' .. '3' ) ( '0' .. '7' ) ( '0' .. '7' )
                    {
                    match('\\'); 
                    // BaseLexer.g:114:14: ( '0' .. '3' )
                    // BaseLexer.g:114:15: '0' .. '3'
                    {
                    matchRange('0','3'); 

                    }

                    // BaseLexer.g:114:25: ( '0' .. '7' )
                    // BaseLexer.g:114:26: '0' .. '7'
                    {
                    matchRange('0','7'); 

                    }

                    // BaseLexer.g:114:36: ( '0' .. '7' )
                    // BaseLexer.g:114:37: '0' .. '7'
                    {
                    matchRange('0','7'); 

                    }


                    }
                    break;
                case 2 :
                    // BaseLexer.g:115:9: '\\\\' ( '0' .. '7' ) ( '0' .. '7' )
                    {
                    match('\\'); 
                    // BaseLexer.g:115:14: ( '0' .. '7' )
                    // BaseLexer.g:115:15: '0' .. '7'
                    {
                    matchRange('0','7'); 

                    }

                    // BaseLexer.g:115:25: ( '0' .. '7' )
                    // BaseLexer.g:115:26: '0' .. '7'
                    {
                    matchRange('0','7'); 

                    }


                    }
                    break;
                case 3 :
                    // BaseLexer.g:116:9: '\\\\' ( '0' .. '7' )
                    {
                    match('\\'); 
                    // BaseLexer.g:116:14: ( '0' .. '7' )
                    // BaseLexer.g:116:15: '0' .. '7'
                    {
                    matchRange('0','7'); 

                    }


                    }
                    break;

            }
        }
        finally {
        }
    }
    // $ANTLR end "OCTAL_ESC"

    // $ANTLR start "UNICODE_ESC"
    public final void mUNICODE_ESC() throws RecognitionException {
        try {
            // BaseLexer.g:121:5: ( '\\\\' 'u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT )
            // BaseLexer.g:121:9: '\\\\' 'u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT
            {
            match('\\'); 
            match('u'); 
            mHEX_DIGIT(); 
            mHEX_DIGIT(); 
            mHEX_DIGIT(); 
            mHEX_DIGIT(); 

            }

        }
        finally {
        }
    }
    // $ANTLR end "UNICODE_ESC"

    // $ANTLR start "OP"
    public final void mOP() throws RecognitionException {
        try {
            // BaseLexer.g:126:2: ( '+' | '-' | '*' | '/' | '%' | '<' | '>' | '<=' | '>=' | '==' | '!=' | '<<' | '>>' | '!' | '~' | '^' )
            int alt37=16;
            alt37 = dfa37.predict(input);
            switch (alt37) {
                case 1 :
                    // BaseLexer.g:126:4: '+'
                    {
                    match('+'); 

                    }
                    break;
                case 2 :
                    // BaseLexer.g:126:8: '-'
                    {
                    match('-'); 

                    }
                    break;
                case 3 :
                    // BaseLexer.g:126:12: '*'
                    {
                    match('*'); 

                    }
                    break;
                case 4 :
                    // BaseLexer.g:126:16: '/'
                    {
                    match('/'); 

                    }
                    break;
                case 5 :
                    // BaseLexer.g:126:20: '%'
                    {
                    match('%'); 

                    }
                    break;
                case 6 :
                    // BaseLexer.g:126:24: '<'
                    {
                    match('<'); 

                    }
                    break;
                case 7 :
                    // BaseLexer.g:126:28: '>'
                    {
                    match('>'); 

                    }
                    break;
                case 8 :
                    // BaseLexer.g:126:32: '<='
                    {
                    match("<="); 


                    }
                    break;
                case 9 :
                    // BaseLexer.g:126:37: '>='
                    {
                    match(">="); 


                    }
                    break;
                case 10 :
                    // BaseLexer.g:126:42: '=='
                    {
                    match("=="); 


                    }
                    break;
                case 11 :
                    // BaseLexer.g:126:47: '!='
                    {
                    match("!="); 


                    }
                    break;
                case 12 :
                    // BaseLexer.g:126:52: '<<'
                    {
                    match("<<"); 


                    }
                    break;
                case 13 :
                    // BaseLexer.g:126:57: '>>'
                    {
                    match(">>"); 


                    }
                    break;
                case 14 :
                    // BaseLexer.g:126:62: '!'
                    {
                    match('!'); 

                    }
                    break;
                case 15 :
                    // BaseLexer.g:126:66: '~'
                    {
                    match('~'); 

                    }
                    break;
                case 16 :
                    // BaseLexer.g:126:70: '^'
                    {
                    match('^'); 

                    }
                    break;

            }
        }
        finally {
        }
    }
    // $ANTLR end "OP"

    public void mTokens() throws RecognitionException {
        // BaseLexer.g:1:8: ( FILTER | JOIN | GLA | PRINT | AGGREGATE | EQUAL | SEPARATOR | ATTRIBUTES | TEXTLOADER | FILE | MATCH_DP | CASE_DP | BOOL_T | ID | INT | FLOAT | COMMENT | WS | STRING | SEMICOLON | COMMA | COLON | DOT | LPAREN | RPAREN | LSQ | RSQ | QMARK | LAND | LOR | BAND | BOR | XOR | LNOT | ISEQUAL | NEQUAL | LS | GT | LE | GE | NOT | SLEFT | SRIGHT | PLUS | MINUS | TIMES | DIVIDE | MOD )
        int alt38=48;
        alt38 = dfa38.predict(input);
        switch (alt38) {
            case 1 :
                // BaseLexer.g:1:10: FILTER
                {
                mFILTER(); 

                }
                break;
            case 2 :
                // BaseLexer.g:1:17: JOIN
                {
                mJOIN(); 

                }
                break;
            case 3 :
                // BaseLexer.g:1:22: GLA
                {
                mGLA(); 

                }
                break;
            case 4 :
                // BaseLexer.g:1:26: PRINT
                {
                mPRINT(); 

                }
                break;
            case 5 :
                // BaseLexer.g:1:32: AGGREGATE
                {
                mAGGREGATE(); 

                }
                break;
            case 6 :
                // BaseLexer.g:1:42: EQUAL
                {
                mEQUAL(); 

                }
                break;
            case 7 :
                // BaseLexer.g:1:48: SEPARATOR
                {
                mSEPARATOR(); 

                }
                break;
            case 8 :
                // BaseLexer.g:1:58: ATTRIBUTES
                {
                mATTRIBUTES(); 

                }
                break;
            case 9 :
                // BaseLexer.g:1:69: TEXTLOADER
                {
                mTEXTLOADER(); 

                }
                break;
            case 10 :
                // BaseLexer.g:1:80: FILE
                {
                mFILE(); 

                }
                break;
            case 11 :
                // BaseLexer.g:1:85: MATCH_DP
                {
                mMATCH_DP(); 

                }
                break;
            case 12 :
                // BaseLexer.g:1:94: CASE_DP
                {
                mCASE_DP(); 

                }
                break;
            case 13 :
                // BaseLexer.g:1:102: BOOL_T
                {
                mBOOL_T(); 

                }
                break;
            case 14 :
                // BaseLexer.g:1:109: ID
                {
                mID(); 

                }
                break;
            case 15 :
                // BaseLexer.g:1:112: INT
                {
                mINT(); 

                }
                break;
            case 16 :
                // BaseLexer.g:1:116: FLOAT
                {
                mFLOAT(); 

                }
                break;
            case 17 :
                // BaseLexer.g:1:122: COMMENT
                {
                mCOMMENT(); 

                }
                break;
            case 18 :
                // BaseLexer.g:1:130: WS
                {
                mWS(); 

                }
                break;
            case 19 :
                // BaseLexer.g:1:133: STRING
                {
                mSTRING(); 

                }
                break;
            case 20 :
                // BaseLexer.g:1:140: SEMICOLON
                {
                mSEMICOLON(); 

                }
                break;
            case 21 :
                // BaseLexer.g:1:150: COMMA
                {
                mCOMMA(); 

                }
                break;
            case 22 :
                // BaseLexer.g:1:156: COLON
                {
                mCOLON(); 

                }
                break;
            case 23 :
                // BaseLexer.g:1:162: DOT
                {
                mDOT(); 

                }
                break;
            case 24 :
                // BaseLexer.g:1:166: LPAREN
                {
                mLPAREN(); 

                }
                break;
            case 25 :
                // BaseLexer.g:1:173: RPAREN
                {
                mRPAREN(); 

                }
                break;
            case 26 :
                // BaseLexer.g:1:180: LSQ
                {
                mLSQ(); 

                }
                break;
            case 27 :
                // BaseLexer.g:1:184: RSQ
                {
                mRSQ(); 

                }
                break;
            case 28 :
                // BaseLexer.g:1:188: QMARK
                {
                mQMARK(); 

                }
                break;
            case 29 :
                // BaseLexer.g:1:194: LAND
                {
                mLAND(); 

                }
                break;
            case 30 :
                // BaseLexer.g:1:199: LOR
                {
                mLOR(); 

                }
                break;
            case 31 :
                // BaseLexer.g:1:203: BAND
                {
                mBAND(); 

                }
                break;
            case 32 :
                // BaseLexer.g:1:208: BOR
                {
                mBOR(); 

                }
                break;
            case 33 :
                // BaseLexer.g:1:212: XOR
                {
                mXOR(); 

                }
                break;
            case 34 :
                // BaseLexer.g:1:216: LNOT
                {
                mLNOT(); 

                }
                break;
            case 35 :
                // BaseLexer.g:1:221: ISEQUAL
                {
                mISEQUAL(); 

                }
                break;
            case 36 :
                // BaseLexer.g:1:229: NEQUAL
                {
                mNEQUAL(); 

                }
                break;
            case 37 :
                // BaseLexer.g:1:236: LS
                {
                mLS(); 

                }
                break;
            case 38 :
                // BaseLexer.g:1:239: GT
                {
                mGT(); 

                }
                break;
            case 39 :
                // BaseLexer.g:1:242: LE
                {
                mLE(); 

                }
                break;
            case 40 :
                // BaseLexer.g:1:245: GE
                {
                mGE(); 

                }
                break;
            case 41 :
                // BaseLexer.g:1:248: NOT
                {
                mNOT(); 

                }
                break;
            case 42 :
                // BaseLexer.g:1:252: SLEFT
                {
                mSLEFT(); 

                }
                break;
            case 43 :
                // BaseLexer.g:1:258: SRIGHT
                {
                mSRIGHT(); 

                }
                break;
            case 44 :
                // BaseLexer.g:1:265: PLUS
                {
                mPLUS(); 

                }
                break;
            case 45 :
                // BaseLexer.g:1:270: MINUS
                {
                mMINUS(); 

                }
                break;
            case 46 :
                // BaseLexer.g:1:276: TIMES
                {
                mTIMES(); 

                }
                break;
            case 47 :
                // BaseLexer.g:1:282: DIVIDE
                {
                mDIVIDE(); 

                }
                break;
            case 48 :
                // BaseLexer.g:1:289: MOD
                {
                mMOD(); 

                }
                break;

        }

    }


    protected DFA25 dfa25 = new DFA25(this);
    protected DFA37 dfa37 = new DFA37(this);
    protected DFA38 dfa38 = new DFA38(this);
    static final String DFA25_eotS =
        "\5\uffff";
    static final String DFA25_eofS =
        "\5\uffff";
    static final String DFA25_minS =
        "\2\56\3\uffff";
    static final String DFA25_maxS =
        "\1\71\1\145\3\uffff";
    static final String DFA25_acceptS =
        "\2\uffff\1\2\1\1\1\3";
    static final String DFA25_specialS =
        "\5\uffff}>";
    static final String[] DFA25_transitionS = {
            "\1\2\1\uffff\12\1",
            "\1\3\1\uffff\12\1\13\uffff\1\4\37\uffff\1\4",
            "",
            "",
            ""
    };

    static final short[] DFA25_eot = DFA.unpackEncodedString(DFA25_eotS);
    static final short[] DFA25_eof = DFA.unpackEncodedString(DFA25_eofS);
    static final char[] DFA25_min = DFA.unpackEncodedStringToUnsignedChars(DFA25_minS);
    static final char[] DFA25_max = DFA.unpackEncodedStringToUnsignedChars(DFA25_maxS);
    static final short[] DFA25_accept = DFA.unpackEncodedString(DFA25_acceptS);
    static final short[] DFA25_special = DFA.unpackEncodedString(DFA25_specialS);
    static final short[][] DFA25_transition;

    static {
        int numStates = DFA25_transitionS.length;
        DFA25_transition = new short[numStates][];
        for (int i=0; i<numStates; i++) {
            DFA25_transition[i] = DFA.unpackEncodedString(DFA25_transitionS[i]);
        }
    }

    class DFA25 extends DFA {

        public DFA25(BaseRecognizer recognizer) {
            this.recognizer = recognizer;
            this.decisionNumber = 25;
            this.eot = DFA25_eot;
            this.eof = DFA25_eof;
            this.min = DFA25_min;
            this.max = DFA25_max;
            this.accept = DFA25_accept;
            this.special = DFA25_special;
            this.transition = DFA25_transition;
        }
        public String getDescription() {
            return "40:1: FLOAT : ( ( '0' .. '9' )+ '.' ( '0' .. '9' )* ( EXPONENT )? ( 'f' | 'L' )? | '.' ( '0' .. '9' )+ ( EXPONENT )? ( 'f' | 'L' )? | ( '0' .. '9' )+ EXPONENT ( 'f' | 'L' )? );";
        }
    }
    static final String DFA37_eotS =
        "\6\uffff\1\16\1\21\1\uffff\1\23\12\uffff";
    static final String DFA37_eofS =
        "\24\uffff";
    static final String DFA37_minS =
        "\1\41\5\uffff\1\74\1\75\1\uffff\1\75\12\uffff";
    static final String DFA37_maxS =
        "\1\176\5\uffff\1\75\1\76\1\uffff\1\75\12\uffff";
    static final String DFA37_acceptS =
        "\1\uffff\1\1\1\2\1\3\1\4\1\5\2\uffff\1\12\1\uffff\1\17\1\20\1\10"+
        "\1\14\1\6\1\11\1\15\1\7\1\13\1\16";
    static final String DFA37_specialS =
        "\24\uffff}>";
    static final String[] DFA37_transitionS = {
            "\1\11\3\uffff\1\5\4\uffff\1\3\1\1\1\uffff\1\2\1\uffff\1\4\14"+
            "\uffff\1\6\1\10\1\7\37\uffff\1\13\37\uffff\1\12",
            "",
            "",
            "",
            "",
            "",
            "\1\15\1\14",
            "\1\17\1\20",
            "",
            "\1\22",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            ""
    };

    static final short[] DFA37_eot = DFA.unpackEncodedString(DFA37_eotS);
    static final short[] DFA37_eof = DFA.unpackEncodedString(DFA37_eofS);
    static final char[] DFA37_min = DFA.unpackEncodedStringToUnsignedChars(DFA37_minS);
    static final char[] DFA37_max = DFA.unpackEncodedStringToUnsignedChars(DFA37_maxS);
    static final short[] DFA37_accept = DFA.unpackEncodedString(DFA37_acceptS);
    static final short[] DFA37_special = DFA.unpackEncodedString(DFA37_specialS);
    static final short[][] DFA37_transition;

    static {
        int numStates = DFA37_transitionS.length;
        DFA37_transition = new short[numStates][];
        for (int i=0; i<numStates; i++) {
            DFA37_transition[i] = DFA.unpackEncodedString(DFA37_transitionS[i]);
        }
    }

    class DFA37 extends DFA {

        public DFA37(BaseRecognizer recognizer) {
            this.recognizer = recognizer;
            this.decisionNumber = 37;
            this.eot = DFA37_eot;
            this.eof = DFA37_eof;
            this.min = DFA37_min;
            this.max = DFA37_max;
            this.accept = DFA37_accept;
            this.special = DFA37_special;
            this.transition = DFA37_transition;
        }
        public String getDescription() {
            return "124:1: fragment OP : ( '+' | '-' | '*' | '/' | '%' | '<' | '>' | '<=' | '>=' | '==' | '!=' | '<<' | '>>' | '!' | '~' | '^' );";
        }
    }
    static final String DFA38_eotS =
        "\1\uffff\12\24\1\101\10\24\1\uffff\1\117\1\121\1\123\12\uffff\1"+
        "\125\1\127\2\uffff\1\131\1\134\1\137\4\uffff\23\24\2\uffff\15\24"+
        "\21\uffff\7\24\3\u008a\27\24\1\u00a2\2\24\1\u00a2\1\24\1\u00a2\3"+
        "\u00a6\1\uffff\15\24\1\u00b4\5\24\3\u00bb\1\24\1\uffff\1\u00b4\2"+
        "\24\1\uffff\3\u00bf\12\24\1\uffff\3\24\3\u00cd\1\uffff\3\u00ce\1"+
        "\uffff\15\24\2\uffff\32\24\1\u00f6\1\24\2\u00f6\2\24\3\u00fa\4\24"+
        "\1\uffff\3\u00ff\1\uffff\4\u0100\2\uffff";
    static final String DFA38_eofS =
        "\u0101\uffff";
    static final String DFA38_minS =
        "\1\11\1\141\1\111\1\157\1\117\1\154\1\114\1\162\1\122\1\147\1\107"+
        "\1\75\1\145\1\105\1\145\1\105\1\141\1\101\1\141\1\101\1\uffff\1"+
        "\56\1\60\1\52\12\uffff\1\46\1\174\2\uffff\1\75\1\74\1\75\4\uffff"+
        "\3\154\1\114\2\151\1\111\2\141\1\101\2\151\1\111\1\147\1\164\1\147"+
        "\1\107\1\164\1\124\2\uffff\2\160\1\120\1\170\1\165\1\170\1\130\2"+
        "\164\1\124\2\163\1\123\21\uffff\1\145\1\163\1\145\1\105\2\156\1"+
        "\116\3\60\2\156\1\116\3\162\1\122\1\162\1\122\2\141\1\101\1\164"+
        "\1\145\1\164\1\124\2\143\1\103\2\145\1\105\1\145\1\60\2\145\1\60"+
        "\1\105\4\60\1\uffff\2\164\1\124\1\145\1\151\1\145\1\105\1\151\1"+
        "\111\2\162\1\122\1\154\1\60\2\114\2\150\1\110\3\60\1\162\1\uffff"+
        "\1\60\1\162\1\122\1\uffff\3\60\1\147\1\142\1\147\1\107\1\142\1\102"+
        "\2\141\1\101\1\157\1\uffff\2\157\1\117\3\60\1\uffff\3\60\1\uffff"+
        "\1\141\1\165\1\141\1\101\1\165\1\125\2\164\1\124\3\141\1\101\2\uffff"+
        "\3\164\1\124\1\164\1\124\2\157\1\117\3\144\1\104\3\145\1\105\1\145"+
        "\1\105\2\162\1\122\3\145\1\105\1\60\1\163\2\60\1\163\1\123\3\60"+
        "\3\162\1\122\1\uffff\3\60\1\uffff\4\60\2\uffff";
    static final String DFA38_maxS =
        "\1\176\2\151\2\157\2\154\2\162\2\164\1\75\2\145\1\162\1\145\4\141"+
        "\1\uffff\1\145\1\71\1\57\12\uffff\1\46\1\174\2\uffff\2\75\1\76\4"+
        "\uffff\3\154\1\114\2\151\1\111\2\141\1\101\2\151\1\111\1\147\1\164"+
        "\1\147\1\107\1\164\1\124\2\uffff\2\160\1\120\1\170\1\165\1\170\1"+
        "\130\2\164\1\124\2\163\1\123\21\uffff\1\164\1\163\1\164\1\124\2"+
        "\156\1\116\3\172\2\156\1\116\3\162\1\122\1\162\1\122\2\141\1\101"+
        "\1\164\1\145\1\164\1\124\2\143\1\103\2\145\1\105\1\145\1\172\2\145"+
        "\1\172\1\105\4\172\1\uffff\2\164\1\124\1\145\1\151\1\145\1\105\1"+
        "\151\1\111\2\162\1\122\1\154\1\172\1\154\1\114\2\150\1\110\3\172"+
        "\1\162\1\uffff\1\172\1\162\1\122\1\uffff\3\172\1\147\1\142\1\147"+
        "\1\107\1\142\1\102\2\141\1\101\1\157\1\uffff\2\157\1\117\3\172\1"+
        "\uffff\3\172\1\uffff\1\141\1\165\1\141\1\101\1\165\1\125\2\164\1"+
        "\124\3\141\1\101\2\uffff\3\164\1\124\1\164\1\124\2\157\1\117\3\144"+
        "\1\104\3\145\1\105\1\145\1\105\2\162\1\122\3\145\1\105\1\172\1\163"+
        "\2\172\1\163\1\123\3\172\3\162\1\122\1\uffff\3\172\1\uffff\4\172"+
        "\2\uffff";
    static final String DFA38_acceptS =
        "\24\uffff\1\16\3\uffff\1\22\1\23\1\24\1\25\1\26\1\30\1\31\1\32\1"+
        "\33\1\34\2\uffff\1\41\1\42\3\uffff\1\54\1\55\1\56\1\60\23\uffff"+
        "\1\43\1\6\15\uffff\1\17\1\20\1\27\1\21\1\57\1\35\1\37\1\36\1\40"+
        "\1\44\1\51\1\47\1\52\1\45\1\50\1\53\1\46\52\uffff\1\3\27\uffff\1"+
        "\12\3\uffff\1\2\15\uffff\1\15\6\uffff\1\14\3\uffff\1\4\15\uffff"+
        "\1\13\1\1\47\uffff\1\5\3\uffff\1\7\4\uffff\1\10\1\11";
    static final String DFA38_specialS =
        "\u0101\uffff}>";
    static final String[] DFA38_transitionS = {
            "\2\30\2\uffff\1\30\22\uffff\1\30\1\46\1\31\2\uffff\1\54\1\42"+
            "\1\31\1\35\1\36\1\53\1\51\1\33\1\52\1\26\1\27\12\25\1\34\1\32"+
            "\1\47\1\13\1\50\1\41\1\uffff\1\12\1\24\1\23\2\24\1\2\1\6\2\24"+
            "\1\4\2\24\1\21\2\24\1\10\2\24\1\15\1\17\6\24\1\37\1\uffff\1"+
            "\40\1\44\1\24\1\uffff\1\11\1\24\1\22\2\24\1\1\1\5\2\24\1\3\2"+
            "\24\1\20\2\24\1\7\2\24\1\14\1\16\6\24\1\uffff\1\43\1\uffff\1"+
            "\45",
            "\1\56\7\uffff\1\55",
            "\1\60\37\uffff\1\57",
            "\1\61",
            "\1\63\37\uffff\1\62",
            "\1\64",
            "\1\66\37\uffff\1\65",
            "\1\67",
            "\1\71\37\uffff\1\70",
            "\1\72\14\uffff\1\73",
            "\1\75\14\uffff\1\77\22\uffff\1\74\14\uffff\1\76",
            "\1\100",
            "\1\102",
            "\1\104\37\uffff\1\103",
            "\1\105\14\uffff\1\106",
            "\1\110\37\uffff\1\107",
            "\1\111",
            "\1\113\37\uffff\1\112",
            "\1\114",
            "\1\116\37\uffff\1\115",
            "",
            "\1\120\1\uffff\12\25\13\uffff\1\120\37\uffff\1\120",
            "\12\120",
            "\1\122\4\uffff\1\122",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "\1\124",
            "\1\126",
            "",
            "",
            "\1\130",
            "\1\133\1\132",
            "\1\135\1\136",
            "",
            "",
            "",
            "",
            "\1\140",
            "\1\141",
            "\1\142",
            "\1\143",
            "\1\144",
            "\1\145",
            "\1\146",
            "\1\147",
            "\1\150",
            "\1\151",
            "\1\152",
            "\1\153",
            "\1\154",
            "\1\155",
            "\1\156",
            "\1\157",
            "\1\160",
            "\1\161",
            "\1\162",
            "",
            "",
            "\1\163",
            "\1\164",
            "\1\165",
            "\1\166",
            "\1\167",
            "\1\170",
            "\1\171",
            "\1\172",
            "\1\173",
            "\1\174",
            "\1\175",
            "\1\176",
            "\1\177",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "\1\u0081\16\uffff\1\u0080",
            "\1\u0082",
            "\1\u0084\16\uffff\1\u0083",
            "\1\u0086\16\uffff\1\u0085",
            "\1\u0087",
            "\1\u0088",
            "\1\u0089",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\1\u008b",
            "\1\u008c",
            "\1\u008d",
            "\1\u008e",
            "\1\u008f",
            "\1\u0090",
            "\1\u0091",
            "\1\u0092",
            "\1\u0093",
            "\1\u0094",
            "\1\u0095",
            "\1\u0096",
            "\1\u0097",
            "\1\u0098",
            "\1\u0099",
            "\1\u009a",
            "\1\u009b",
            "\1\u009c",
            "\1\u009d",
            "\1\u009e",
            "\1\u009f",
            "\1\u00a0",
            "\1\u00a1",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\1\u00a3",
            "\1\u00a4",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\1\u00a5",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "",
            "\1\u00a7",
            "\1\u00a8",
            "\1\u00a9",
            "\1\u00aa",
            "\1\u00ab",
            "\1\u00ac",
            "\1\u00ad",
            "\1\u00ae",
            "\1\u00af",
            "\1\u00b0",
            "\1\u00b1",
            "\1\u00b2",
            "\1\u00b3",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\1\u00b6\37\uffff\1\u00b5",
            "\1\u00b7",
            "\1\u00b8",
            "\1\u00b9",
            "\1\u00ba",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\1\u00bc",
            "",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\1\u00bd",
            "\1\u00be",
            "",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\1\u00c0",
            "\1\u00c1",
            "\1\u00c2",
            "\1\u00c3",
            "\1\u00c4",
            "\1\u00c5",
            "\1\u00c6",
            "\1\u00c7",
            "\1\u00c8",
            "\1\u00c9",
            "",
            "\1\u00ca",
            "\1\u00cb",
            "\1\u00cc",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "",
            "\1\u00cf",
            "\1\u00d0",
            "\1\u00d1",
            "\1\u00d2",
            "\1\u00d3",
            "\1\u00d4",
            "\1\u00d5",
            "\1\u00d6",
            "\1\u00d7",
            "\1\u00d8",
            "\1\u00d9",
            "\1\u00da",
            "\1\u00db",
            "",
            "",
            "\1\u00dc",
            "\1\u00dd",
            "\1\u00de",
            "\1\u00df",
            "\1\u00e0",
            "\1\u00e1",
            "\1\u00e2",
            "\1\u00e3",
            "\1\u00e4",
            "\1\u00e5",
            "\1\u00e6",
            "\1\u00e7",
            "\1\u00e8",
            "\1\u00e9",
            "\1\u00ea",
            "\1\u00eb",
            "\1\u00ec",
            "\1\u00ed",
            "\1\u00ee",
            "\1\u00ef",
            "\1\u00f0",
            "\1\u00f1",
            "\1\u00f2",
            "\1\u00f3",
            "\1\u00f4",
            "\1\u00f5",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\1\u00f7",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\1\u00f8",
            "\1\u00f9",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\1\u00fb",
            "\1\u00fc",
            "\1\u00fd",
            "\1\u00fe",
            "",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "\12\24\7\uffff\32\24\4\uffff\1\24\1\uffff\32\24",
            "",
            ""
    };

    static final short[] DFA38_eot = DFA.unpackEncodedString(DFA38_eotS);
    static final short[] DFA38_eof = DFA.unpackEncodedString(DFA38_eofS);
    static final char[] DFA38_min = DFA.unpackEncodedStringToUnsignedChars(DFA38_minS);
    static final char[] DFA38_max = DFA.unpackEncodedStringToUnsignedChars(DFA38_maxS);
    static final short[] DFA38_accept = DFA.unpackEncodedString(DFA38_acceptS);
    static final short[] DFA38_special = DFA.unpackEncodedString(DFA38_specialS);
    static final short[][] DFA38_transition;

    static {
        int numStates = DFA38_transitionS.length;
        DFA38_transition = new short[numStates][];
        for (int i=0; i<numStates; i++) {
            DFA38_transition[i] = DFA.unpackEncodedString(DFA38_transitionS[i]);
        }
    }

    class DFA38 extends DFA {

        public DFA38(BaseRecognizer recognizer) {
            this.recognizer = recognizer;
            this.decisionNumber = 38;
            this.eot = DFA38_eot;
            this.eof = DFA38_eof;
            this.min = DFA38_min;
            this.max = DFA38_max;
            this.accept = DFA38_accept;
            this.special = DFA38_special;
            this.transition = DFA38_transition;
        }
        public String getDescription() {
            return "1:1: Tokens : ( FILTER | JOIN | GLA | PRINT | AGGREGATE | EQUAL | SEPARATOR | ATTRIBUTES | TEXTLOADER | FILE | MATCH_DP | CASE_DP | BOOL_T | ID | INT | FLOAT | COMMENT | WS | STRING | SEMICOLON | COMMA | COLON | DOT | LPAREN | RPAREN | LSQ | RSQ | QMARK | LAND | LOR | BAND | BOR | XOR | LNOT | ISEQUAL | NEQUAL | LS | GT | LE | GE | NOT | SLEFT | SRIGHT | PLUS | MINUS | TIMES | DIVIDE | MOD );";
        }
    }
 

}