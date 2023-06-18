# Communicating Finite-State Machines System Simulator (CFSMSS)

## TODO

- LTL expressions
- Errors and message losses
- Timeouts
- GUI

## Text CFSMSS format

Borrowed and adapted from <http://www.fsmtestonline.ru/>
>fsms 2 - number of FSMs, it is two here  
msgs 4 - number of messages

The next block is repeated for each fsm, so 2 times in this example.

>s 4 - number of states in FSM  
n0 0 - initial state  
m0 1 - index of an FSM in a memory slot at the beginning of a simulation
p 8 - number of transitions  
0 0 1 1 - transition: starting state, incomming message, end state, output message  
0 1 2 1 - next transition  
1 2 1 - transition without an outgoing message
...

After that a list of external messages is given.

>external_msgs 5  
0 CLIENT 2-REQ - send message "2-REQ" to fsm CLIENT before the first step.  
2 SERVER 2-DONE - send "2-DONE" to the fsm "SERVER" before step number 2.  
...

States, inputs and outputs are not necessary integers.
>s 4 START END CONNECT DISCONNECT - After the states’ number the list of the state names is given  
n0 START - The initial state  
m0 SERVER - FSM SERVER will recieve the first message if it is external
p 8
START INPUT-1 DISCONNECT OUTPUT-1 - A transition is a 4-tuple: Starting state ‘START’, input ‘INPUT-1’, end state ‘DISCONNECT’ and output ‘OUTPUT-1'  
START INPUT-2 START OUTPUT-2  
...

Have a look at the examples.

## References

- D. Brand and P. Zafiropulo. On communicating finite-state machines. Journal of the ACM, 30(2):323-342, 1983.
- Abdulla, Parosh Aziz; Jonsson, Bengt (10 October 1996). "Undecidable Verification Problems for Programs with Unreliable Channels". Information and Computation. 130 (1): 71–90. doi:10.1006/inco.1996.0083.
- Benedikt Bollig, Marie Fortin, Paul Gastin. Communicating finite-state machines, first-order logic, and star-free propositional dynamic logic. Journal of Computer and System Sciences, Volume 115, 2021, Pages 22-53, ISSN 0022-0000, <https://doi.org/10.1016/j.jcss.2020.06.006>.
- TU-TS. ITU-TS Recommendation Z.120: Message Sequence Chart 1999 (MSC99). Technical report, ITU-TS, Geneva, 1999
