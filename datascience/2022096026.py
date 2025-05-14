import sys

def load_transactions(input_file):
    transactions = []
    with open(input_file, 'r') as f:
        for line in f:
            transactions.append(set(map(int, line.strip().split('\t'))))
    return transactions

def get_frequent_itemsets(transactions, min_support):
    item_counts = {}
    total_transactions = len(transactions)

    for transaction in transactions:
        for item in transaction:
            item_counts[item] = item_counts.get(item, 0) + 1

    frequent_itemsets = {
        frozenset([item]): count / total_transactions * 100
        for item, count in item_counts.items() if (count / total_transactions * 100) >= min_support
    }

    k = 2
    while True:
        candidates = {}
        frequent_items = list(frequent_itemsets.keys())

        for i in range(len(frequent_items)):
            for j in range(i + 1, len(frequent_items)):
                candidate = frequent_items[i] | frequent_items[j]
                if len(candidate) == k:
                    count = 0
                    for t in transactions:
                        if candidate.issubset(t):
                            count += 1
                    support = count / total_transactions * 100
                    if support >= min_support:
                        candidates[candidate] = support

        if not candidates:
            break

        frequent_itemsets.update(candidates)
        k += 1

    return frequent_itemsets

def get_subsets(s):
    subsets = []
    s = list(s)
    n = len(s)
    for i in range(1, 1 << n):
        subset = set()
        for j in range(n):
            if i & (1 << j):
                subset.add(s[j])
        if len(subset) < len(s):
            subsets.append(frozenset(subset))
    return subsets

def generate_association_rules(frequent_itemsets):
    rules = []

    for itemset in frequent_itemsets:
        if len(itemset) < 2:
            continue

        subsets = get_subsets(itemset)
        for subset in subsets:
            remaining = itemset - subset
            if remaining:
                support = frequent_itemsets[itemset]
                confidence = support / frequent_itemsets[subset] * 100
                rules.append((subset, remaining, support, confidence))

    return rules

def save_output(output_file, rules):
    with open(output_file, 'w') as f:
        for antecedent, consequent, support, confidence in rules:
            f.write(f"{{{','.join(map(str, antecedent))}}}\t{{{','.join(map(str, consequent))}}}\t{support:.2f}\t{confidence:.2f}\n")

def main():
    if len(sys.argv) != 4:
        print("Usage: python studentID.py <min_support> <input_file> <output_file>")
        return

    min_support = float(sys.argv[1])
    input_file = sys.argv[2]
    output_file = sys.argv[3]

    transactions = load_transactions(input_file)
    frequent_itemsets = get_frequent_itemsets(transactions, min_support)
    rules = generate_association_rules(frequent_itemsets)
    save_output(output_file, rules)

if __name__ == "__main__":
    main()
