GF_TEMPLATE_DESC(</AndGF/>)
dnl
m4_define(</AndGF/>, </dnl
m4_divert_push(-1)

m4_redefine(</MY_NAME/>, </$1/>)

M4_COLLECT_STATES(m4_shift($@))

m4_ifdef_undef(</MY_INIT/>)
m4_ifdef_undef(</MY_REQ_STATES/>)
m4_ifdef_undef(</MY_GEN_STATES/>)

m4_redefine(</MY_INPUTS/>, m4_quote(m4_defn(</__INPUTS/>)))
m4_redefine(</INNER_GFS/>, m4_quote(m4_defn(</__STATES/>)))
m4_ifval(m4_quote(__INITS), </m4_redefine(</MY_INIT/>, m4_quote(m4_defn(</__INITS/>)))/>)
m4_ifval(m4_quote(__REQ_STATES), </m4_redefine(</MY_REQ_STATES/>, m4_quote(m4_defn(</__REQ_STATES/>)))/>)

m4_define(</MY_GEN_STATES/>, </(myConstState, $1_ConstState)/>)

m4_divert_pop(-1)dnl
</#/>ifndef MY_NAME<//>_H_
</#/>define MY_NAME<//>_H_

/*  Meta-information
 *  GF_DESC
 *      NAME(</$1/>)
 *      INPUTS(MY_INPUTS)
m4_ifdef(</MY_INIT/>, </dnl
 *      CONSTRUCTOR(MY_INIT)
/>)dnl
 *      GEN_CONST_STATES(MY_GEN_STATES)
m4_ifdef(</MY_REQ_STATES/>, </dnl
 *      REQ_CONST_STATES(MY_REQ_STATES)
/>)dnl
 *  END_DESC
 */

M4_CONTAINER_TEMPLATE(MY_NAME, </dnl
    bool Filter( TYPED_CONST_REF_ARGS(MY_INPUTS) ) {
        return true<//>dnl
m4_foreach(</_S_/>, m4_quote(INNER_GFS), </dnl
 && VAR(_S_).Filter(ARGS(m4_quote(VAR(_S_)</_T_INPUT/>)))dnl
/>)dnl
;
    }
/>)dnl

</#/>endif // MY_NAME</_H_/>
/>)dnl
