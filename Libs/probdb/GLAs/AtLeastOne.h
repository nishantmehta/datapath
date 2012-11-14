#ifndef _ATLEASTONE_H_
#define _ATLEASTONE_H_

/** Info for the meta-GLAs
 * GLA_DESC
 *
 * NAME(</AtLeastOne/>)
 * INPUTS(</(p, DOUBLE)/>)
 * OUTPUTS(</(p_out, DOUBLE)/>)
 * RESULT_TYPE(</single/>)
 *
 * END_DESC
 */

class AtLeastOne{
	long double prod; // product of (1-p)
public:
	AtLeastOne(void){ prod=1.0; }
	void AddItem(DOUBLE p){ prod*=(1.0-p); /* cout << "AtLeastOne p="<<p<<" prod="<<prod<< endl; */}
	void AddState(AtLeastOne& o){ prod*=o.prod; }
	void Finalize(){ /* nothing */ }
	void GetResult(DOUBLE& p){ p=1-prod; /*cout << "AtLeastOne prod="<<prod<< endl; */}
};

#endif  // _ATLEASTONE_H_
