import createCalculatorModule from "../wasm/calculator.js";

const expressionInput = document.querySelector("#expression");
const resultOutput = document.querySelector("#result");
const messageOutput = document.querySelector("#message");
const engineStatus = document.querySelector("#engineStatus");
const statusDot = document.querySelector("#statusDot");
const historyList = document.querySelector("#history");

let calculatorModule = null;
const calculationHistory = [];

function setStatus(message, state) {
    engineStatus.textContent = message;
    statusDot.className = "status-dot";

    if (state) {
        statusDot.classList.add(state);
    }
}

function showMessage(message, isError = false) {
    messageOutput.textContent = message;
    messageOutput.classList.toggle("error", isError);
}

function formatResult(value) {
    if (!Number.isFinite(value)) {
        return String(value);
    }

    return new Intl.NumberFormat("en-US", {
        maximumSignificantDigits: 12
    }).format(value);
}

function renderHistory() {
    historyList.replaceChildren();

    if (calculationHistory.length === 0) {
        const item = document.createElement("li");
        item.className = "empty-history";
        item.textContent = "Your calculations will appear here.";
        historyList.append(item);
        return;
    }

    for (const entry of calculationHistory) {
        const item = document.createElement("li");

        const expression = document.createElement("span");
        expression.className = "history-expression";
        expression.textContent = entry.expression;

        const value = document.createElement("span");
        value.className = "history-value";
        value.textContent = entry.value;

        item.append(expression, value);
        historyList.prepend(item);
    }
}

function calculate() {
    if (!calculatorModule) {
        showMessage("The calculation engine is not ready.", true);
        return;
    }

    const expression = expressionInput.value.trim();

    if (!expression) {
        showMessage("Enter an expression first.", true);
        expressionInput.focus();
        return;
    }

    try {
        const response = calculatorModule.evaluateExpression(expression);

        if (!response.success) {
            resultOutput.textContent = "—";
            showMessage(response.error || "Calculation failed.", true);
            return;
        }

        const formatted = formatResult(response.value);
        resultOutput.textContent = formatted;
        showMessage("Calculation completed successfully.");

        calculationHistory.unshift({
            expression,
            value: formatted
        });

        if (calculationHistory.length > 8) {
            calculationHistory.pop();
        }

        renderHistory();
    } catch (error) {
        console.error("Calculation error:", error);
        resultOutput.textContent = "—";
        showMessage("Unable to evaluate this expression.", true);
    }
}

function insertAtCursor(value) {
    const start = expressionInput.selectionStart;
    const end = expressionInput.selectionEnd;
    const current = expressionInput.value;

    expressionInput.value =
        current.slice(0, start) + value + current.slice(end);

    const cursor = start + value.length;
    expressionInput.setSelectionRange(cursor, cursor);
    expressionInput.focus();
}

document.querySelectorAll("[data-insert]").forEach((button) => {
    button.addEventListener("click", () => {
        insertAtCursor(button.dataset.insert);
    });
});

document.querySelector("#equals").addEventListener("click", calculate);

document.querySelector("#clear").addEventListener("click", () => {
    expressionInput.value = "";
    resultOutput.textContent = "0";
    showMessage("Ready for a new calculation.");
    expressionInput.focus();
});

document.querySelector("#backspace").addEventListener("click", () => {
    const start = expressionInput.selectionStart;
    const end = expressionInput.selectionEnd;
    const current = expressionInput.value;

    if (start !== end) {
        expressionInput.value = current.slice(0, start) + current.slice(end);
        expressionInput.setSelectionRange(start, start);
    } else if (start > 0) {
        expressionInput.value =
            current.slice(0, start - 1) + current.slice(end);
        expressionInput.setSelectionRange(start - 1, start - 1);
    }

    expressionInput.focus();
});

document.querySelector("#clearHistory").addEventListener("click", () => {
    calculationHistory.length = 0;
    renderHistory();
});

expressionInput.addEventListener("keydown", (event) => {
    if (event.key === "Enter") {
        event.preventDefault();
        calculate();
    }

    if (event.key === "Escape") {
        expressionInput.value = "";
        resultOutput.textContent = "0";
        showMessage("Ready for a new calculation.");
    }
});

async function initializeCalculator() {
    try {
        calculatorModule = await createCalculatorModule();

        setStatus("C++ engine ready", "ready");
        showMessage("Ready. Enter an expression to begin.");
        expressionInput.focus();
    } catch (error) {
        console.error("WebAssembly initialization failed:", error);
        setStatus("Engine failed to load", "error");
        showMessage(
            "Could not load the WebAssembly engine. Check the browser console.",
            true
        );
    }
}

renderHistory();
initializeCalculator();
