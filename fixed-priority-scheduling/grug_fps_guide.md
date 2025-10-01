# GRUG GUIDE TO FIXED PRIORITY SCHEDULING 🦴

## What is Fixed Priority Scheduling (FPS)?

**Each task get priority number before program run. Scheduler always pick highest priority task that ready.**

- Priority decided offline (before runtime)
- During runtime: highest priority ready task runs
- Most widely used scheduling in real world
- Tasks execute in priority order (subject to their periods)

**Why grug like:**
- Simple algorithm, low overhead - O(n) complexity
- Easy scale to many tasks
- Easy handle tasks with different periods
- When system overload, only low priority tasks miss deadlines (high priority still ok)

**Why grug not like:**
- High priority tasks can starve low priority ones (low priority never run)
- More complex than cyclic executive

---

## How Assign Priority?

**IMPORTANT: In real-time, "priority" based on TIMING needs, NOT importance!**

**Two optimal ways to assign priorities:**

### 1. Rate Monotonic (RM)
**Smallest period → highest priority**

### 2. Deadline Monotonic (DM)
**Smallest deadline → highest priority**

*If deadlines equal periods (D=T), RM and DM give same priorities*

### Example:

| Task | C (ms) | T (ms) | D (ms) | RM Priority | DM Priority |
|------|--------|--------|--------|-------------|-------------|
| Control | 20 | 60 | 40 | High (3) | Med (2) |
| Alarm | 5 | 70 | 20 | Med (2) | High (3) |
| Logger | 50 | 100 | 100 | Low (1) | Low (1) |

**Deadline Monotonic is optimal when D ≠ T**

**Note:** Some OSes use inverted numbering (lower number = higher priority)

---

## Critical Instant - When Worst Case Happen?

**Critical instant = time when task experiences maximum interference**

### Preemptive (no blocking):
**All tasks released at t=0** = worst case

Why? Task sees maximum interference when all higher priority tasks arrive same time.

### Non-preemptive:
**Just before task (and all higher priority) released, longest lower-priority task starts executing**

Why? Task must wait for blocking task to finish, THEN wait for all higher priority tasks.

---

## Schedulability Analysis - Is System OK?

**Need verify all tasks meet deadlines: ∀i ⇒ Ri ≤ Di**

**Two types of tests:**
1. Utilization-based (simple but pessimistic)
2. Response-time analysis (exact but need iterate)

---

## Method 1: Utilization-Based Test

### Simple Formula:

```
U = Σ(Ci/Ti)    (sum utilization of all tasks)
```

### For Preemptive FPS (when D=T):

- **If U > 1** → System NEVER schedulable (need more than 100% CPU)
- **If U ≤ n(2^(1/n) - 1)** → System ALWAYS schedulable
  - n=1: U≤100%
  - n=2: U≤83%
  - n=3: U≤78%
  - n→∞: U≤69%
- **Between these values** → Cannot conclude (need response-time analysis)

**This is SUFFICIENT but NOT NECESSARY test**
- If pass → definitely schedulable ✓
- If fail → maybe schedulable, maybe not 🤷

### Example:

| Task | C | T | U(Ci/Ti) |
|------|---|---|----------|
| τ₁ | 2 | 10 | 0.20 |
| τ₂ | 9 | 15 | 0.60 |
| τ₃ | 1 | 25 | 0.04 |
| **Total** | | | **0.84 = 84%** |

For n=3: bound is 78%
U=84% > 78% → Cannot conclude from utilization test alone
Need response-time analysis to know for sure

### Problem with Utilization Test:

**Very pessimistic!** Many schedulable systems fail this test but actually work fine.

---

## Method 2: Response-Time Analysis (RTA)

**Calculate exact worst-case response time (WCRT) for each task**

**If Ri ≤ Di for all tasks → System schedulable ✓**

This is **NECESSARY AND SUFFICIENT** test (exact, not pessimistic)

### Preemptive Response-Time Formula:

```
Ri = Ci + Σ(⌈Ri/Tj⌉ × Cj)    for all j ∈ hp(i)
```

Where:
- `Ri` = response time of task i
- `Ci` = execution time of task i
- `Tj` = period of higher priority task j
- `Cj` = execution time of higher priority task j
- `⌈Ri/Tj⌉` = number of times task j interferes (ceiling function)
- `hp(i)` = set of tasks with higher priority than i

**Problem: Ri appears on BOTH sides!**

Must solve with **iterative fixed-point technique**

---

## How to Iterate Response Time

### Step 1: Initialize
```
R⁰ᵢ = Cᵢ
```
Start assuming no interference (optimistic)

### Step 2: Iterate
```
R^(n+1)ᵢ = Cᵢ + Σ(⌈R^n_i/Tj⌉ × Cj)
```
Keep calculating, using previous R value

### Step 3: Stop When

**Converged:**
```
R^(n+1)ᵢ = R^n_i
```
Found stable response time! Check if Ri ≤ Di

**OR Diverged:**
```
R^(n+1)ᵢ > Di
```
Response time exceeded deadline! Task NOT schedulable ✗

---

## Why Response Times Increase During Iteration

**Response times form NON-DECREASING sequence:**

```
R⁰ ≤ R¹ ≤ R² ≤ ... ≤ R^n = R^(n+1)
```

**Why?**
- As Ri increases, ⌈Ri/Tj⌉ can only stay same or increase
- More interference → higher response time
- Cannot decrease because adding positive values

**If sequence were decreasing, that would be impossible** (can't have less interference with more time)

---

## Detailed Example - Preemptive

| Task | C (ms) | T=D (ms) |
|------|--------|----------|
| τ₁ | 1 | 6 |
| τ₂ | 12 | 130 |
| τ₃ | 5 | 140 |

**Calculate R₃:**

**Iteration 0:**
```
R⁰₃ = C₃ = 5
```

**Iteration 1:**
```
R¹₃ = 5 + ⌈5/6⌉×1 + ⌈5/130⌉×12
    = 5 + 1×1 + 1×12
    = 18
```
*(Added 1 job of τ₁ and 1 job of τ₂)*

**Iteration 2:**
```
R²₃ = 5 + ⌈18/6⌉×1 + ⌈18/130⌉×12
    = 5 + 3×1 + 1×12
    = 20
```
*(Now 3 jobs of τ₁ interfere)*

**Iteration 3:**
```
R³₃ = 5 + ⌈20/6⌉×1 + ⌈20/130⌉×12
    = 5 + 4×1 + 1×12
    = 21
```
*(Now 4 jobs of τ₁)*

**Iteration 4:**
```
R⁴₃ = 5 + ⌈21/6⌉×1 + ⌈21/130⌉×12
    = 5 + 4×1 + 1×12
    = 21
```

**CONVERGED! R³₃ = R⁴₃ = 21**

**Check schedulability:**
```
R₃ = 21 ≤ D₃ = 140  ✓ Schedulable!
```

---

## Non-Preemptive Response-Time Analysis

**When task start, it run to completion (no preemption)**

**Key difference:** Higher priority tasks only interfere if they arrive BEFORE task starts

**Must add BLOCKING TIME (Bi):**

```
Bi = max(Cj)    for all j ∈ lp(i)
```

Blocking = longest execution time of any lower-priority task

**Why?** Just before task i released, longest low-priority task might grab CPU and block it.

### Formula:

**Step 1:** Calculate interference I
```
I⁰ᵢ = Bᵢ

I^(n+1)ᵢ = Bᵢ + Σ(⌈I^n_i/Tj⌉ × Cj)    for all j ∈ hp(i)
```

**Step 2:** Calculate response time
```
Ri = Ii + Ci
```

**Iterate I until converges, then add Ci**

---

## Utilization Test for Non-Preemptive

**More complex because must check each task individually:**

For constrained deadline taskset, task τᵢ schedulable if:

```
(Ci + Bi)/Di + 1 × ∏(Uj + 1) ≤ 2
                   j∈hp(i)
```

Where ∏ = product of all higher priority task utilizations

**Tests become pessimistic and complex** → Response-time analysis better for non-preemptive

---

## Complete Schedulability Algorithm

### For each task i (from highest to lowest priority):

1. **Calculate Ri using iteration**
2. **Check if Ri ≤ Di**
   - If YES → Task i schedulable, continue to next task
   - If NO → Task i NOT schedulable, **system fails** ✗

**ALL tasks must pass for system to be schedulable**

---

## Ceiling vs Floor Function Debate

**Old way (still commonly used):**
```
Ri = Ci + Σ⌈Ri/Tj⌉ × Cj
```

**Correct way (proven later):**
```
Ri = Ci + Σ(⌊Ri/Tj⌋ + 1) × Cj
```

**Why?**
- If two tasks arrive simultaneously, cannot both execute
- Ceiling of zero is zero (⌈0/T⌉ = 0)
- But if tasks arrive together, at least one must wait
- Floor+1 correctly accounts for this

**In practice:** Difference usually small. Most literature still uses ceiling.

---

## Summary for Grug

### Priority Assignment:
- **Rate Monotonic:** Shorter period = higher priority
- **Deadline Monotonic:** Shorter deadline = higher priority

### Quick Check - Utilization:
- Calculate U = Σ(Ci/Ti)
- If U > 1 → definitely NOT schedulable
- If U ≤ 69% (many tasks) → definitely schedulable
- Otherwise → need response-time analysis

### Exact Check - Response Time:
1. Start with Ri = Ci
2. Iterate: Ri = Ci + Σ⌈Ri/Tj⌉×Cj (all higher priority j)
3. Stop when Ri stops changing OR Ri > Di
4. If Ri ≤ Di → task schedulable
5. Must check ALL tasks

### Convergence:
- Response times **increase or stay same** (never decrease)
- This guarantees convergence or divergence (no infinite loop)
- If converge → system predictable
- If diverge → system NOT schedulable

### Non-Preemptive:
- Add blocking time Bi = max execution of lower priority tasks
- Iterate on interference Ii, then Ri = Ii + Ci

---

**Grug say: Start simple with U test. If not sure, use response-time iteration. Always check ALL tasks.**
