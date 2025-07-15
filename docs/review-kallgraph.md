# A Critical Review of “Redefining Indirect Call Analysis with KallGraph” [1]


The paper *KallGraph* [1] introduces a hybrid analysis technique that integrates point-to analysis into the MLTA [4] framework to enhance precision in resolving indirect calls. Central to its contribution is the claim of identifying “fundamental design flaws” in MLTA.

We initially shared a version of this review with the authors of *KallGraph* on **June 16, 2025**. They responded to parts of our evaluation, and we have taken their feedback into account in this updated review. This revised version reflects our continued efforts to ensure the technical accuracy of the discussion and to clarify misconceptions related to MLTA and its successors.

---

## TL;DR Summary

- Most major claims of *KallGraph* are not supported by evidence, and its experimental results appear significantly off.
- MLTA does not exhibit the claimed type confinement issues; its design and experiments confirm sound handling of such cases.
- All example cases used to demonstrate MLTA’s “unsoundness” are incorrect; MLTA does not miss the reported targets. These include examples in Figure 3 and Section 7.2.1.
- Of 66 false negatives reported in *KallGraph*’s Table 6, only 17 are verifiable; none are due to design flaws.
- *KallGraph* itself shows a substantially higher FN rate than reported—missing 1,782 of 6,500 traced indirect calls. Precision claims become pointless with such a high FN rate.
- Table 2 states *KallGraph* completes Linux kernel analysis in 4.5 hours. In fact, it requires 270 CPU hours; MLTA takes only 0.5 CPU hour.

---

## 1. KallGraph’s Soundness Claims Against MLTA

*KallGraph* presents two main claims of unsoundness in MLTA, both attributed to its design.

### 1.1 Alleged Unsound Type Confinement Rule

*KallGraph* claims that MLTA’s “Type Confinement Rule” is unsound because it cannot track layered struct types across function boundaries. However, this is incorrect---MLTA includes escape analysis and propagation policy to address exactly this case.

From Section 4.1.3 of the MLTA paper:

> One thing to note is that, when we cannot decide if a composite type would be stored or cast to an unsupported type, e.g., a pointer of an object of the composite type is passed to or from other functions, and we cannot decide how this pointer is used in those functions, we will also treat the composite type as escaping.

This mechanism ensures soundness for interprocedural ambiguity, but *KallGraph* does not account for it in its critique.

#### Supportive experiments

*KallGraph* uses Figure 3 to argue that MLTA misses the target function `b_read` for `icall2`. In fact, MLTA captures this target correctly. See the screenshot below:


*KallGraph* also claims 66 of its 100 FNs are due to this issue. Our reanalysis finds that:

- 61 are not false negatives at all.
- The remaining 5 are caused by an implementation error (see Section 2).

**Update:** 
The authors responded to this correction: they acknowledged their example is incorrect and tried to provide a new example. It turns out the new example is irrelevant to the claimed unsound confinement, but that “ReturnInst” was not handled yet in “typeConfineInFunction()” of MLTA.

---

### 1.2 Alleged Unsound Type Propagation Rule

The paper also raises questions about MLTA’s “Type Propagation Rule” and claims it is a design flaw which we disagree with. 
We would like to clarify that this is MLTA’s decision to not adopt the bi-directional propagation for typecast for a clear reason:
The scenario, in practice, does NOT exist. In our experiments, we’ve never encountered such a case. The `unsound_cast` example 
provided in figure 3 of the paper is unrealistic. There is no reason to intermediately introduce object `f` of the essentially 
same type (exact same fields) when both source and use are for `e`. Interestingly, all the 15 false negatives mentioned in the 
Table 6 are not due to the issue; instead they are caused by broken types (see details in section 2) from the LLVM compiler.

Again, by running its example in Figure 3, our experiment shows that MLTA does not miss the target f_read().


---

### 1.3 Discrepancy in Reported FN Rate

*KallGraph* claims to have only 9 FNs among 2,937 targets (937 traced + 2,000 sampled). Using full tracing on Linux 5.18, we observed 6,461 ground-truth indirect call targets.

Findings:

- *KallGraph* missed 1,812 targets (1,782 with `mem2reg`).
- The authors have not responded to this discrepancy.
- MLTA initially had 342 FNs (mostly from an improper syscall-pointer-array handling), and only 33 FNs after the fix.
- TFA [2] had just 2 FNs under the same conditions.

📄 **[50_samples_of_FN_of_KallGraph_in_Linux_5.18.txt](docs/50_samples_of_FN_of_KallGraph_in_Linux_5.18.txt)** contains our FN samples of KallGraph.

---

## 2. Detailed Re-evaluation of the Reported False Negatives

*KallGraph* lists 66 MLTA FNs in Table 6. Our reevaluation using MLTA shows:

- Only 17 are verifiable FNs.
- 49 are resolved correctly.
- Both examples in Section 7.2.1 (`link->doit()`, `ia32_sys_call_table[unr]`) are correctly handled by MLTA.

*KallGraph* attributes the 17 FNs to:

- 5: “unsound type confinement”
- 7: “unsound typecast handling”
- 5: “weak implementation”

Our investigation shows they stem from LLVM type issues and implementation artifacts---not from MLTA’s design.

📄 **[re-evaluation-kallgraph.txt](docs/re-evaluation-kallgraph.txt)** provides a case-by-case breakdown.

---

### 2.1 Root Causes of False Negatives: A Correction

We categorize the 17 confirmed FNs into two root causes:

#### 2.1.1 Known LLVM Type System Issues

LLVM IR may represent the same C struct inconsistently across modules.

Example (`io_uring/io_uring.c:1867`):

- In `io_uring/io_uring.bc`:
  ```llvm
  %struct.io_issue_def = type { i16, i32 (%struct.io_kiocb*, i32)*, i32 (%struct.io_kiocb*, %struct.io_uring_sqe*)* }

MLTA uses name-based type comparison, which fails here. *KallGraph* refers to this as “unsound typecast handling,” but it is a well-known limitation of LLVM IR and has been discussed in prior work [2, 3, 5]. MLTA’s successors address this issue through structural matching.

---

### 2.1.2 Implementation Artifacts

A specific issue involves the use of `Ctx->GlobalFuncMap` inside `typeConfineInFunction()`. This map was not fully populated at the time of analysis, which prevented MLTA from triggering its fallback behavior for indirect calls. 

Reordering the LLVM pass to ensure the map is populated beforehand resolves the issue.

These are **implementation limitations**, not **design flaws**.

---

## 3. Other Major Comments

### 3.1 On MLTA’s Relationship to KallGraph

*KallGraph* repeatedly claims MLTA is an “ad hoc” version of itself. We respectfully disagree.

MLTA is a principled type-based analysis that complements point-to analysis. While *KallGraph* combines both, this does not imply that either individual technique is ad hoc.

MLTA’s primary contribution lies in **structural type reasoning**, not in its **minimized data-flow** analysis.

---

### 3.2 On KallGraph’s Fixed-Point Optimization

*KallGraph* introduces a reachability-based fixed-point optimization to reduce iterations. However, this optimization assumes one of two things:

1. A sound call graph is available **before** the analysis begins—which *KallGraph* doesn't have initially, or  
2. The call graph must be computed **iteratively**—which negates the benefit of the proposed optimization.

This circular dependency undermines the theoretical soundness and practical effectiveness of the optimization.

**Update**: Authors responded to this by claiming that KallGraph uses flow-insensitive and context-insensitive analysis, so doesn't suffer from this issue. 
Apparently, this answer doesn't address the concern.

---

## References

[1] Guoren Li, Manu Sridharan, and Zhiyun Qian. *Redefining Indirect Call Analysis with KallGraph*. IEEE S&P 2025, pp. 2734–2752.  
[2] Dinghao Liu, Shouling Ji, Kangjie Lu, and Qinming He. *Improving Indirect-Call Analysis in LLVM with Type and Data-Flow Co-Analysis*. USENIX Security 2024.  
[3] Kangjie Lu. *Practical Program Modularization with Type-Based Dependence Analysis*. IEEE S&P 2023.  
[4] Kangjie Lu and Hong Hu. *Where Does It Go? Refining Indirect-Call Targets with Multi-Layer Type Analysis*. ACM CCS 2019.  
[5] Tianrou Xia, Hong Hu, and Dinghao Wu. *DeepType: Refining Indirect Call Targets with Strong Multi-Layer Type Analysis*. USENIX Security 2024.
