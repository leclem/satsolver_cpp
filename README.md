    ===== SAT Solver =====


1 ) How to use the program
---------------------------
To make the sat solver work, you just need to give him an input file beginning with  

p cnf for the classic SAT, followed by the clauses in the Conjunctive Normal Form (Dimacs format)

p logic for the tseitin transformation. You can write complex clauses that will be transforme in a classic SAT problem and solved by our program. 
Here is how you should write these clauses  
The AND is represented by /\ or & , the OR by \/ or | , the logical consequence by =>, and the not by !   
Example : (5 & 4) => var2  

To compile, just type make in the terminal to use the makefile. The generated file is called sat2 and is located in the /bin folder.  
To give an input file, just lace it in the stdin or give it as an arument to sat2.  
The way of giving argument is stipulated in the help :  
Syntax: sat [option] [-flags] [<filename in>] [-o <filename out>]  
With one of these options:  
 --DPLL        (by default), use DPLL algorithm  
 --rand        DPLL with random acces  
 --MOMS        MOMS heuristic  
 --DLIS        DLIS heuristic  
 --hepl        show this help  
 --hist        open a prompt to parse history.slh (learned clauses)  
And the following flags:  
-w      use the watched literals  
-l      use learned clauses  
If the l flag is set, these others flags can be used too:  
-i      disable interactive mode  
-d      disable documentation file generation  

2) Strengths and weaknesses
---------------------------
We think one of the advantages of our solver is the diversity of the possibilities of resolution of sat. You can launch it in DPLL, DLIS, MOMS, or rand mode with or without watched literals, with or without learned clauses, witch gives us 16 different heuristics. Our code is very modular, so one can easily add new heuristics
.

The weakness of our code is the optimisation of these heuristics. Indeed, always manipulating our code to make it modularisable made us abandon a lot of optimisations that we didn't made again because of the time.
