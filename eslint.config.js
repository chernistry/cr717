import js from '@eslint/js';
import tsPlugin from '@typescript-eslint/eslint-plugin';
import tsParser from '@typescript-eslint/parser';
import prettier from 'eslint-config-prettier';

export default [
  js.configs.recommended,
  {
    ignores: ['dist/**', 'node_modules/**'],
  },
  {
    files: ['**/*.ts'],
    languageOptions: {
      parser: tsParser,
      parserOptions: {
        ecmaVersion: 2020,
        sourceType: 'module',
      },
      globals: {
        console: 'readonly',
        document: 'readonly',
        window: 'readonly',
        setTimeout: 'readonly',
        clearTimeout: 'readonly',
        setInterval: 'readonly',
        clearInterval: 'readonly',
        requestAnimationFrame: 'readonly',
        cancelAnimationFrame: 'readonly',
        global: 'readonly',
        HTMLElement: 'readonly',
        HTMLElementTagNameMap: 'readonly',
        HTMLButtonElement: 'readonly',
        HTMLInputElement: 'readonly',
        AudioContext: 'readonly',
        OfflineAudioContext: 'readonly',
        AudioBuffer: 'readonly',
        OscillatorNode: 'readonly',
        GainNode: 'readonly',
        BiquadFilterNode: 'readonly',
        AudioBufferSourceNode: 'readonly',
        Float32Array: 'readonly',
      },
    },
    plugins: {
      '@typescript-eslint': tsPlugin,
    },
    rules: {
      ...tsPlugin.configs.recommended.rules,
      '@typescript-eslint/no-unused-vars': 'error',
      '@typescript-eslint/explicit-function-return-type': [
        'warn',
        { allowExpressions: true },
      ],
    },
  },
  prettier,
];
